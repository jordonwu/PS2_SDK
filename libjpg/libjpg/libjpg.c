#include <tamtypes.h>
#include <kernel.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <screenshot.h>

#include <libjpg.h>
#include "jpeglib.h"

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  u8 *dest;
  int size;
			
  u8 *buffer;
} _destination_mgr;

typedef struct {
  /* libjpg private fields */
  struct jpeg_decompress_struct dinfo;
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  struct jpeg_source_mgr jsrc;
  _destination_mgr jdst;

  JSAMPLE *buffer;
  u8 *data;
  u8 *dest;
} jpgPrivate;

FILE* dLog=0;

void __Log(char *fmt, ...) {
	char buf[1024];
	va_list list;

/*	if (dLog == 0) {
		dLog = fopen("host:dLog.txt", "w");
	}
*/			
	va_start(list, fmt);
	vsprintf(buf, fmt, list);
	va_end(list);
	printf("%s", buf);
//	fwrite( buf, strlen(buf), 1, dLog);

//	fclose(dLog);
}


void _src_init_source(j_decompress_ptr cinfo) {
}

boolean _src_fill_input_buffer(j_decompress_ptr cinfo) {
  return TRUE;
}

void _src_skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
}

boolean _src_resync_to_restart(j_decompress_ptr cinfo, int desired) {
  return TRUE;
}

void _src_term_source(j_decompress_ptr cinfo) {
}

void _src_output_message(j_common_ptr cinfo) {
  char buffer[JMSG_LENGTH_MAX];

  /* Create the message */
  (*cinfo->err->format_message) (cinfo, buffer);

#ifdef DEBUG
  printf("%s\n", buffer);
#endif
}

void _src_error_exit(j_common_ptr cinfo) {
  /* Always display the message */
  (*cinfo->err->output_message) (cinfo);

  /* Let the memory manager delete any temp files before we die */
  jpeg_destroy(cinfo);

//  exit(EXIT_FAILURE);
}


jpgData *jpgOpenRAW(u8 *data, int size) {
  struct jpeg_decompress_struct *dinfo;
  jpgPrivate *priv;
  jpgData *jpg;

  jpg = malloc(sizeof(jpgData));
  if (jpg == NULL) return NULL;

  memset(jpg, 0, sizeof(jpgData));
  priv = malloc(sizeof(jpgPrivate));
  if (priv == NULL) return NULL;

  jpg->priv = priv;
  dinfo = &priv->dinfo;
  /* Initialize the JPEG decompression object with default error handling. */
  dinfo->err = jpeg_std_error(&priv->jerr);
  jpeg_create_decompress(dinfo);

  /* Specify data source for decompression */
  priv->jsrc.next_input_byte   = data;
  priv->jsrc.bytes_in_buffer   = size;
  priv->jsrc.init_source       = _src_init_source;
  priv->jsrc.fill_input_buffer = _src_fill_input_buffer;
  priv->jsrc.skip_input_data   = _src_skip_input_data;
  priv->jsrc.resync_to_restart = _src_resync_to_restart;
  priv->jsrc.term_source       = _src_term_source;
  dinfo->src = &priv->jsrc;

  /* Read file header, set default decompression parameters */
  jpeg_read_header(dinfo, TRUE);

  /* Calculate output image dimensions so we can allocate space */
  jpeg_calc_output_dimensions(dinfo);

  priv->buffer = (*dinfo->mem->alloc_small)
      ((j_common_ptr) dinfo, JPOOL_IMAGE,
       dinfo->output_width * dinfo->out_color_components);

  /* Start decompressor */
  jpeg_start_decompress(dinfo);

  jpg->width  = dinfo->output_width;
  jpg->height = dinfo->output_height;
  jpg->bpp    = dinfo->out_color_components*8;

  /* All done. */
  return(priv->jerr.num_warnings ? NULL : jpg);
}

jpgData *jpgOpen(char *filename) {
  jpgData *jpg;
  jpgPrivate *priv;
  u8 *data;
  int size;
  FILE* file;

  file = fopen(filename, "r");
  if (file == 0) {
    printf("jpgOpen: error opening '%s'\n", filename);
    return NULL;
  }
  size = fseek(file, 0, SEEK_END);
  fseek(file, 0, SEEK_SET);
  data = (u8*)malloc(size);
  if (data == NULL) return NULL;
  fread(data, size, 1, file);
  fclose(file);

  jpg = jpgOpenRAW(data, size);
  if (jpg == NULL) return NULL;
  priv = jpg->priv;
  priv->data = data;

  return jpg;
}

int  jpgReadImage(jpgData *jpg, u8 *dest) {
  JDIMENSION num_scanlines;
  jpgPrivate *priv = jpg->priv;
  int width = priv->dinfo.output_width * priv->dinfo.out_color_components;

  /* Process data */
  while (priv->dinfo.output_scanline < priv->dinfo.output_height) {
    num_scanlines = jpeg_read_scanlines(&priv->dinfo, &priv->buffer, 1);
	if (num_scanlines != 1) break;
	memcpy(dest, priv->buffer, priv->dinfo.output_width * priv->dinfo.out_color_components);
	dest+= width;
  }

  /* Finish decompression and release memory.
   * I must do it in this order because output module has allocated memory
   * of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
   */
  jpeg_finish_decompress(&priv->dinfo);
  jpeg_destroy_decompress(&priv->dinfo);

  return(priv->jerr.num_warnings ? -1 : 0);
}

#define OUTPUT_BUF_SIZE	(64*1024)

void _dest_init_destination(j_compress_ptr cinfo) {
//__Log("_dest_init_destination\n");
}

boolean _dest_empty_output_buffer(j_compress_ptr cinfo) {
  _destination_mgr *dest = (_destination_mgr*) cinfo->dest;

//__Log("_dest_empty_output_buffer, %d\n", dest->size);
  dest->dest = realloc(dest->dest, dest->size+OUTPUT_BUF_SIZE);
  memcpy(dest->dest+dest->size, dest->buffer, OUTPUT_BUF_SIZE);

  cinfo->dest->next_output_byte = dest->buffer;
  cinfo->dest->free_in_buffer   = OUTPUT_BUF_SIZE;

  dest->size+= OUTPUT_BUF_SIZE;

  return TRUE;
}

void _dest_term_destination(j_compress_ptr cinfo) {
  _destination_mgr *dest = (_destination_mgr*) cinfo->dest;
  int size = OUTPUT_BUF_SIZE - cinfo->dest->free_in_buffer;

//__Log("_dest_term_destination, %d\n", dest->size);
  dest->dest = realloc(dest->dest, dest->size+size);
  memcpy(dest->dest+dest->size, dest->buffer, size);

  dest->size+= size;
}

jpgData *jpgCreateRAW(u8 *data, int width, int height, int bpp) {
  struct jpeg_compress_struct *cinfo;
  jpgPrivate *priv;
  jpgData *jpg;

//__Log("jpgCreateRAW\n");
  jpg = malloc(sizeof(jpgData));
  if (jpg == NULL) return NULL;

//__Log("jpgCreateRAW1\n");
  memset(jpg, 0, sizeof(jpgData));
  priv = malloc(sizeof(jpgPrivate));
  if (priv == NULL) return NULL;

//__Log("jpgCreateRAW2\n");
  jpg->priv = priv;
  cinfo = &priv->cinfo;

  /* Initialize the JPEG compression object with default error handling. */
  cinfo->err = jpeg_std_error(&priv->jerr);
  jpeg_create_compress(cinfo);

//__Log("jpgCreateRAW3\n");
  priv->data = data;
  priv->buffer = malloc(OUTPUT_BUF_SIZE);
  if (priv->buffer == NULL) return NULL;
//__Log("jpgCreateRAW3 %p\n", priv->buffer);

  priv->jdst.pub.next_output_byte = priv->buffer;
  priv->jdst.pub.free_in_buffer   = OUTPUT_BUF_SIZE;
  priv->jdst.pub.init_destination = _dest_init_destination;
  priv->jdst.pub.empty_output_buffer = _dest_empty_output_buffer;
  priv->jdst.pub.term_destination = _dest_term_destination;
  priv->jdst.buffer = priv->buffer;

//printf("%p, %p\n", &priv->jdst.pub.next_output_byte, &priv->jdst);
  /* Specify data source for decompression */
  cinfo->dest = (struct jpeg_destination_mgr *)&priv->jdst;

//__Log("jpgCreateRAW4\n");
  cinfo->image_width = width; 	/* image width and height, in pixels */
  cinfo->image_height = height;
  if (bpp == 8) {
    cinfo->input_components = 1;	/* # of color components per pixel */
    cinfo->in_color_space = JCS_GRAYSCALE; /* colorspace of input image */
  } else {
    cinfo->input_components = 3;	/* # of color components per pixel */
    cinfo->in_color_space = JCS_RGB;    /* colorspace of input image */
  }

//__Log("jpgCreateRAW5\n");
  jpeg_set_defaults(cinfo);
  jpeg_start_compress(cinfo, TRUE);

  jpg->width = cinfo->image_width;
  jpg->height = cinfo->image_height;
  jpg->bpp = cinfo->input_components*8;
//__Log("jpgCreateRAW6\n");

  /* All done. */
  return(priv->jerr.num_warnings ? NULL : jpg);
}

int jpgCompressImageRAW(jpgData *jpg, u8 **dest) {
  jpgPrivate *priv = jpg->priv;
  _destination_mgr *_dest = (_destination_mgr*) priv->cinfo.dest;
  int width = priv->cinfo.image_width * priv->cinfo.input_components;
  JSAMPROW row_pointer[1];	/* pointer to a single row */

  _dest->dest = NULL;
  _dest->size = 0;

//__Log("jpgCompressImageRAW\n");
  while (priv->cinfo.next_scanline < priv->cinfo.image_height) {
    row_pointer[0] = & priv->data[priv->cinfo.next_scanline * width];
//__Log("jpgCompressImageRAW %d %x (%d)\n", priv->cinfo.next_scanline, row_pointer[0], priv->jdst.pub.free_in_buffer);
    jpeg_write_scanlines(&priv->cinfo, row_pointer, 1);
  }
//__Log("jpgCompressImageRAW2\n");
  jpeg_finish_compress(&priv->cinfo);
  *dest = _dest->dest;

  return(priv->jerr.num_warnings ? -1 : _dest->size);
}

void jpgClose(jpgData *jpg) {
  jpgPrivate *priv = jpg->priv;
  if (priv->data) free(priv->data);
  free(priv);
  free(jpg);
}

#define PS2SS_GSPSMCT32           0
#define PS2SS_GSPSMCT24           1
#define PS2SS_GSPSMCT16           2

int jpgScreenshot( const char* pFilename,unsigned int VramAdress,
                         unsigned int Width, unsigned int Height, unsigned int Psm )
{
  FILE* file_handle;
  u32 y;
  static u32 in_buffer[1024*4];  // max 1024*32bit for a line, should be ok
  u8 *out_buffer;
  u8 *p_out;
  jpgData *jpg;
  u8 *data;
  int ret;

  file_handle = fopen( pFilename, "w" );

  // make sure we could open the file for output

  if( file_handle == 0 )
    return 0;

  out_buffer = malloc( Width*Height*3 );
  if( out_buffer == NULL )
    return 0;
  p_out = out_buffer;

  /////////////////////////////////////////////////////////////////////////////
  // Check if we have a tempbuffer, if we do we use it 

  for( y = 0; y < Height; y++ )
  {
    ps2_screenshot( in_buffer, VramAdress, 0, y, Width, 1, Psm );

    if( Psm == PS2SS_GSPSMCT16 )
    {
      u32 x;
      u16* p_in  = (u16*)&in_buffer;
        
      for( x = 0; x < Width; x++ )
      {
	 u32 r = (p_in[x] & 31) << 3;
	 u32 g = ((p_in[x] >> 5) & 31) << 3;
	 u32 b = ((p_in[x] >> 10) & 31) << 3;
         p_out[x*3+0] = r;
         p_out[x*3+1] = g;
         p_out[x*3+2] = b;
      }
    }
    else
    if( Psm == PS2SS_GSPSMCT24 )
    {
      u32 x;
      u8* p_in  = (u8*)&in_buffer;
        
      for( x = 0; x < Width; x++ )
      {
        u8 r =  *p_in++;
        u8 g =  *p_in++;
        u8 b =  *p_in++;
        p_out[x*3+0] = r;
        p_out[x*3+1] = g;
        p_out[x*3+2] = b;
      }
    }
    else
    {
       u8 *p_in = (u8 *) &in_buffer;
       u32 x;

       for(x = 0; x < Width; x++)
       {
	  u8 r = *p_in++;
	  u8 g = *p_in++;
	  u8 b = *p_in++;
	  *p_in++;
          p_out[x*3+0] = r;
          p_out[x*3+1] = g;
          p_out[x*3+2] = b;
       }
    }

    p_out+= Width*3;
  }

  jpg = jpgCreateRAW(out_buffer, Width, Height, 24);
  ret = jpgCompressImageRAW(jpg, &data);
  fwrite( data, ret, 1, file_handle );
  jpgClose(jpg);

  fclose( file_handle );

  free(out_buffer);

  return 0;
}

