/*
 * memory protocol interface
 */

#include <stdlib.h>
#include "libavutil/error.h"
#include "libavutil/log.h"
#include "avformat.h"
#include "url.h"

void ffvcl_reset_memory_protocol_callback(void);
void ffvcl_set_memory_protocol_callback(
    int     (*open_cb )(URLContext *, const char          *, int),
    int     (*read_cb )(URLContext *, unsigned char       *, int),
    int     (*write_cb)(URLContext *, const unsigned char *, int),
    int64_t (*seek_cb )(URLContext *, int64_t              , int),
    int     (*close_cb)(URLContext *                            ));

static int     (*open_callback )(URLContext *, const char          *, int) = NULL;
static int     (*read_callback )(URLContext *, unsigned char       *, int) = NULL;
static int     (*write_callback)(URLContext *, const unsigned char *, int) = NULL;
static int64_t (*seek_callback )(URLContext *, int64_t              , int) = NULL;
static int     (*close_callback)(URLContext *                            ) = NULL;

void ffvcl_reset_memory_protocol_callback(void)
{
    open_callback  = NULL;
    read_callback  = NULL;
    write_callback = NULL;
    seek_callback  = NULL;
    close_callback = NULL;
}

void ffvcl_set_memory_protocol_callback(
    int     (*open_cb )(URLContext *, const char          *, int),
    int     (*read_cb )(URLContext *, unsigned char       *, int),
    int     (*write_cb)(URLContext *, const unsigned char *, int),
    int64_t (*seek_cb )(URLContext *, int64_t              , int),
    int     (*close_cb)(URLContext *))
{
    open_callback  = open_cb;
    read_callback  = read_cb;
    write_callback = write_cb;
    seek_callback  = seek_cb;
    close_callback = close_cb;
}

typedef struct MemoryContext {
    const URLContext *h;
    void *opaque;
	int flags;
} MemoryContext;

static int memory_open(URLContext *h, const char *filename, int flags)
{
    MemoryContext *c = h->priv_data;
    int ret;
    //av_strstart(filename, "memory:", &filename);
    if (open_callback) {
		c->h = NULL;
        c->opaque = NULL;
		c->flags = 0;
        ret = open_callback(h, filename, flags);
    } else {
        av_log(h, AV_LOG_FATAL, "open_callback() is NULL.\n");
        ret = AVERROR(EINVAL);
    }
    return ret;
}

static int memory_read(URLContext *h, unsigned char *buf, int size)
{
    int ret;
    if (read_callback) {
        ret = read_callback(h, buf, size);
    } else {
        av_log(h, AV_LOG_FATAL, "read_callback() is NULL.\n");
        ret = AVERROR(EINVAL);
    }
    return ret;
}

static int memory_write(URLContext *h, const unsigned char *buf, int size)
{
    int ret;
    if (write_callback) {
        ret = write_callback(h, buf, size);
    } else {
        av_log(h, AV_LOG_FATAL, "write_callback() is NULL.\n");
        ret = AVERROR(EINVAL);
    }
    return ret;
}

static int64_t memory_seek(URLContext *h, int64_t pos, int whence)
{
    int64_t ret;
    if (seek_callback) {
        ret = seek_callback(h, pos, whence);
    } else {
        av_log(h, AV_LOG_FATAL, "seek_callback() is NULL.\n");
        ret = AVERROR(EINVAL);
    }
    return ret;
}

static int memory_close(URLContext *h)
{
    int ret;
    if (close_callback) {
        ret = close_callback(h);
    } else {
        av_log(h, AV_LOG_FATAL, "close_callback() is NULL.\n");
        ret = AVERROR(EINVAL);
    }
    return ret;
}

const URLProtocol ff_memory_protocol = {
    .name                = "memory",
    .url_open            = memory_open,
    .url_read            = memory_read,
    .url_write           = memory_write,
    .url_seek            = memory_seek,
    .url_close           = memory_close,
    .priv_data_size      = sizeof(MemoryContext),
    .default_whitelist   = "memory"
};

