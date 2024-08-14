#include "circular_buffer.h"
#include "memory.h"
#include "malloc.h"
PCIRCULAR_BUFFER paudio_circular_buffer;
static circular_buffer audio_circular_buffer = {0};

#ifdef _TUSB_CONFIG_H_
// DMA_RCV_CHANNEL
PDMA_RCV_BUFFER paudio_dma_rcv_buffer;
static dma_rcv_buffer audio_dma_rcv_buffer = {0,0,(CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX*CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE/1000/CFG_TUD_AUDIO_FUNC_1_N_TX_SUPP_SW_FIFO),{0,}};

PDMA_RCV_BUFFER AX_InitDmaBuffer(void){
    paudio_dma_rcv_buffer = &audio_dma_rcv_buffer;
    return paudio_dma_rcv_buffer;
}

#endif


static inline uint16_t __ax_enumerate_tree(buffer_tree* tree){
    if(tree == NULL)
        return 0u;

    if(tree->nodes == NULL)
        return 0u;
    
    uint8_t endflg = 0u;    
    buffer_tree_node* node =  tree->nodes;
    node->node_idx = 0;
    tree->nodes_cnt = 1;
    while(!endflg){
        node = node->next_node;
        if(node == NULL){
            endflg = 1u;
            break;
        }        
        node->node_idx = (++(tree->nodes_cnt));
    }
    return tree->nodes_cnt;
}

static inline uint8_t __ax_initial_check_nodes(buffer_tree* tree, buffer_tree_node* node){
    if(
        (tree == NULL) | (node == NULL)
    )
        return 0xE;

    if(tree->nodes == NULL){
        node->node_idx = 0;
        tree->nodes = node;
        tree->root_node = node;
        tree->last_node = node;
        tree->nodes_cnt = 1u;
        return 0xA;
    }

    return 0x0;
}

static inline void __ax_add_node_last(buffer_tree* tree, buffer_tree_node* node){

    if(__ax_initial_check_nodes(tree, node))
        return;

    buffer_tree_node* last_node = tree->last_node;
    
    if(last_node == NULL)
        return; // ERR_NOMEM
    
    node->node_idx =(++(tree->nodes_cnt))-1;
    node->prev_node = last_node;
    node->next_node = NULL;

    last_node->next_node = node;
    tree->last_node = node;
}

static inline void __ax_add_node_first(buffer_tree* tree, buffer_tree_node* node){

   if(__ax_initial_check_nodes(tree, node))
        return;    
    buffer_tree_node* first_node = tree->root_node;
    (++(tree->nodes_cnt));
    node->node_idx = 0;    
    node->prev_node = NULL;
    node->next_node = first_node;
    first_node->prev_node = node;

    tree->root_node = node;
    tree->nodes = node;
    
    __ax_enumerate_tree(tree);
}

static inline buffer_tree_node* __ax_get_tree_node_by_idx(buffer_tree* tree, uint16_t idx){
    if (tree->nodes_cnt <= 0)
        return NULL;

    if(idx >= tree->nodes_cnt)
        return tree->last_node;

    uint8_t lboolFnd = 0x0u;
    buffer_tree_node* node = (idx >= (tree->nodes_cnt/2)) ? tree->last_node : tree->root_node;

    while (!lboolFnd){
        if(node->node_idx == idx){
            lboolFnd = 0x1u;
            break;
        }
        node = (idx >= (tree->nodes_cnt/2)) ? node->prev_node : node->next_node ;
    }
    return node;
}

static inline buffer_tree_node* AX_CreateNewNode(void){
     buffer_tree_node* treenode = (buffer_tree_node*) malloc(sizeof(buffer_tree_node));
     sample_audio_buffer* buffer = (sample_audio_buffer*) malloc(sizeof(sample_audio_buffer));
     treenode->node_idx = 0u;
     treenode->pbuffer = buffer;
     treenode->next_node = NULL;
     treenode->prev_node = NULL;
     return treenode;
}


void AX_InitCircualBuffer(void){ 
    audio_circular_buffer.buffer_tree = (buffer_tree*)  malloc(sizeof(buffer_tree));
    audio_circular_buffer.buffer_tree->nodes_cnt = 0;
    audio_circular_buffer.buffer_tree->root_node = NULL;
    audio_circular_buffer.buffer_tree->last_node = NULL;
    audio_circular_buffer.buffer_tree->nodes = NULL;
    audio_circular_buffer.rd_pos = 0u;
    audio_circular_buffer.wr_pos = 0u;
    audio_circular_buffer.latest_wr_node = NULL;
    audio_circular_buffer.latest_rd_node = NULL;

    audio_circular_buffer.current_buffer_sz = 32;
    paudio_circular_buffer = &audio_circular_buffer;
    // TESTS
    // a. tree add nodes last
    // for(uint16_t i = 0; i< 8; i++){
    //     buffer_tree_node* node = AX_CreateNewNode(); 
    //     __ax_add_node_last(audio_circular_buffer.buffer_tree, node);
    // }
    // b. tree add nodes first
    // for(uint16_t i = 0; i< 8; i++){
    //     buffer_tree_node* node = AX_CreateNewNode();
    //     __ax_add_node_first(audio_circular_buffer.buffer_tree, node);
    // }

    // c. circular write test
    // #define BUFFER_SZ_U16 48
    //     for(uint16_t i = 0; i<96u; i++){
    //         uint16_t pSamples[BUFFER_SZ_U16] = {0,};
    //         for( uint16_t jidx = 0; jidx < BUFFER_SZ_U16; jidx+=2 ){
    //             pSamples[jidx] = (i+jidx) + 0;
    //             pSamples[jidx+1] = (i+jidx) + 1;
    //         }        
    //         __ax_wr_sample(pSamples, BUFFER_SZ_U16);
    //     }
    // e. circular buffer decode 2xu16 to 1xu32
    // #define BUFFER_SZ_U16 48u
    // #define WRITES_CNT 64u
    // for(uint16_t i = 0; i<WRITES_CNT; i++){
    //     uint16_t pSamples[BUFFER_SZ_U16] = {0,};
    //     for( uint16_t jidx = 0; jidx < BUFFER_SZ_U16; jidx+=2 ){
    //         pSamples[jidx] = (i+jidx) + 0;
    //         pSamples[jidx+1] = (i+jidx) + 1;
    //     }        
    //     __ax_wr_sample(pSamples, BUFFER_SZ_U16);
    // }
    // // from first position
    // extern void __ax_encode_u16sample_to_u32sample(const uint16_t rd_sample);
    // for(uint16_t i = 0; i < (WRITES_CNT/2); i++){
    //     __ax_encode_u16sample_to_u32sample(i);
    // }
    // // from last position
    // for(uint16_t i = (WRITES_CNT/2); i < (WRITES_CNT); i++){
    //     __ax_encode_u16sample_to_u32sample(i);
    // }
    
    // d. circular buffer read test
}

void __ax_encode_u16sample_to_u32sample(const uint16_t rd_sample){
    buffer_tree* tree = audio_circular_buffer.buffer_tree;
    buffer_tree_node* node = __ax_get_tree_node_by_idx(tree, rd_sample);
    // decode 2 uint16_t samples to one uint32_t sample;
    uint16_t bit_samples_sz32 = sizeof(node->pbuffer->ONE_SAMPLE_DATA_32) / sizeof(node->pbuffer->ONE_SAMPLE_DATA_32[0]);
    for(uint16_t i = 0; i < bit_samples_sz32; i++){
        node->pbuffer->ONE_SAMPLE_DATA_32[i] = 
            ((uint32_t)node->pbuffer->ONE_SAMPLE_DATA_16[2 * i] << 16) | node->pbuffer->ONE_SAMPLE_DATA_16[2 * i + 1];
    }
}

void __ax_wr_sample(uint16_t* sample_buffer, uint16_t buffer_sz){
    buffer_tree* tree = audio_circular_buffer.buffer_tree;
    uint16_t tree_sz = tree->nodes_cnt;
    buffer_tree_node* nextnode;
    // add or find element
    if(tree_sz >= audio_circular_buffer.current_buffer_sz){
            if(audio_circular_buffer.latest_wr_node->node_idx >= (tree_sz-1)){
                nextnode = audio_circular_buffer.buffer_tree->root_node;
            } else
            {
                nextnode = audio_circular_buffer.latest_wr_node->next_node;
            }
            audio_circular_buffer.latest_wr_node = nextnode;
    }
    else{
        if(
            (audio_circular_buffer.latest_wr_node == NULL) | (audio_circular_buffer.latest_wr_node->next_node == NULL)
        ){
            nextnode = AX_CreateNewNode();
            __ax_add_node_last(audio_circular_buffer.buffer_tree, nextnode);
            audio_circular_buffer.latest_wr_node = nextnode;
        }
        else{ // !NULL, не создаем
            nextnode = audio_circular_buffer.latest_wr_node->next_node;
            audio_circular_buffer.latest_wr_node = nextnode;
        }
    }
    // write samples from sample buffer to circular buffer
    memcpy(((void*)&(nextnode->pbuffer->ONE_SAMPLE_DATA_16[0])), (void*)(&sample_buffer[0]), sizeof(sample_buffer[0])*buffer_sz); // src
    audio_circular_buffer.wr_pos = nextnode->node_idx;    
}

void __ax_wr_first_half_sample(uint16_t* sample_buffer, uint16_t buffer_sz){
    buffer_tree* tree = audio_circular_buffer.buffer_tree;
    uint16_t tree_sz = tree->nodes_cnt;
    buffer_tree_node* nextnode;
    // add or find element
    if(tree_sz >= audio_circular_buffer.current_buffer_sz){
            if(audio_circular_buffer.latest_wr_node->node_idx >= (tree_sz-1)){
                nextnode = audio_circular_buffer.buffer_tree->root_node;
            } else
            {
                nextnode = audio_circular_buffer.latest_wr_node->next_node;
            }
            audio_circular_buffer.latest_wr_node = nextnode;
    }
    else{
        if(
            (audio_circular_buffer.latest_wr_node == NULL) | (audio_circular_buffer.latest_wr_node->next_node == NULL)
        ){
            nextnode = AX_CreateNewNode();
            __ax_add_node_last(audio_circular_buffer.buffer_tree, nextnode);
            audio_circular_buffer.latest_wr_node = nextnode;
        }
        else{ // !NULL, не создаем
            nextnode = audio_circular_buffer.latest_wr_node->next_node;
            audio_circular_buffer.latest_wr_node = nextnode;
        }
    }
    // write samples from sample buffer to circular buffer
    memcpy(((void*)&(nextnode->pbuffer->ONE_SAMPLE_DATA_16[0])), (void*)(&sample_buffer[0]), sizeof(sample_buffer[0])*buffer_sz); // src
    audio_circular_buffer.wr_pos = nextnode->node_idx;    
}

void __ax_wr_last_half_sample(uint16_t* sample_buffer, uint16_t buffer_sz){
    buffer_tree_node* currentnode;
    currentnode = audio_circular_buffer.latest_wr_node;
    memcpy(
        (void*)&(currentnode->pbuffer->ONE_SAMPLE_DATA_16[
                sizeof(currentnode->pbuffer->ONE_SAMPLE_DATA_16[0])*buffer_sz/2
            ]), 
        (void*)(&sample_buffer[(buffer_sz/2)]), (sizeof(sample_buffer[0])*buffer_sz)/2
    ); // src
}

// I2S DMA Half and Cplt Callbacks
void Circular_HalfCpltCallBack(void){
    __ax_wr_first_half_sample(paudio_dma_rcv_buffer->ONE_SAMPLE_DATA_16, paudio_dma_rcv_buffer->buffer_sample_size);
}

void Circular_CpltCallBack(void){
    __ax_wr_last_half_sample(paudio_dma_rcv_buffer->ONE_SAMPLE_DATA_16, paudio_dma_rcv_buffer->buffer_sample_size);    
}