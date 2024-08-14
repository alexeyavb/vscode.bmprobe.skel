#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "tusb_config.h"

#ifndef _TUSB_CONFIG_H_
struct _sample_audio_buffer{
    uint16_t ONE_SAMPLE_DATA_16[48];            // 48 samples in 1ms one 32 bit in 2 16bit frame
    uint32_t ONE_SAMPLE_DATA_32[24];            // 24 samples in 1ms one2one in 32bit samples

#else
// default value of buffer
struct _dma_rcv_buffer{
    uint32_t tHalfStamp, tCpltStamp; // Time stamps for Half and Cplt
    // (CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX*CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE/1000/CFG_TUD_AUDIO_FUNC_1_N_TX_SUPP_SW_FIFO)*2;
    const uint16_t buffer_sample_size ;
    uint16_t ONE_SAMPLE_DATA_16[(CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX*CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE/1000/CFG_TUD_AUDIO_FUNC_1_N_TX_SUPP_SW_FIFO)];           // 96 16bit samples
};
typedef struct _dma_rcv_buffer dma_rcv_buffer, *PDMA_RCV_BUFFER;
extern PDMA_RCV_BUFFER paudio_dma_rcv_buffer;
extern PDMA_RCV_BUFFER AX_InitDmaBuffer(void);

struct _sample_audio_buffer{    
    uint16_t ONE_SAMPLE_DATA_16[(CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX*CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE/1000/CFG_TUD_AUDIO_FUNC_1_N_TX_SUPP_SW_FIFO)];           // 96 samples in 1ms one 32 bit in 2 16bit frame
    uint32_t ONE_SAMPLE_DATA_32[(CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX*CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE/1000/CFG_TUD_AUDIO_FUNC_1_N_TX_SUPP_SW_FIFO)/2];         // 48 samples in 1ms one2one in 32bit samples
};
#endif
typedef struct _sample_audio_buffer sample_audio_buffer, *PSAMPLE_AUDIO_BUFFER;

struct _buffer_tree_node{
    uint16_t node_idx;
    // audio data
    sample_audio_buffer *pbuffer;
    // struct _buffer_tree_node *root_node;
    struct _buffer_tree_node *prev_node;
    struct _buffer_tree_node *next_node;
};
typedef struct _buffer_tree_node buffer_tree_node, *PBUFFER_TREE_NODE;

struct _buffer_tree{
    uint16_t nodes_cnt;
    buffer_tree_node *nodes;
    buffer_tree_node *root_node;
    buffer_tree_node *last_node;
};
typedef struct _buffer_tree buffer_tree, *PBUFFER_TREE;

struct _circular_buffer{
    uint16_t wr_pos;
    uint16_t rd_pos;
    uint16_t initial_buffer_sz;
    uint16_t current_buffer_sz;
    buffer_tree_node *latest_rd_node;  // cache current rd_node
    buffer_tree_node *latest_wr_node;  // cache current rd_node
    buffer_tree *buffer_tree;
};
typedef struct _circular_buffer circular_buffer, *PCIRCULAR_BUFFER;

extern PCIRCULAR_BUFFER paudio_circular_buffer;
extern void AX_InitCircualBuffer(void);
extern void __ax_wr_sample(uint16_t* sample_buffer, const uint16_t buffer_sz);
extern void __ax_encode_u16sample_to_u32sample(const uint16_t rd_sample);
extern void Circular_HalfCpltCallBack(void);
extern void Circular_CpltCallBack(void);
#ifdef __cplusplus
}
#endif

#endif /* __CIRCULAR_BUFFER_H__ */

