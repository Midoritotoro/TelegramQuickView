#pragma once

#include "../../../core/Types.h"
#include "../../../core/Time.h"

#include "Enums.h"

#include <atomic>

extern "C" {
    #include <libswscale/swscale.h>
}


namespace FFmpeg {
    using fourcc_t = uint32_t;
    typedef void (*ancillary_free_cb)(void* data);

    struct module_t;
    struct plagin_t;
    struct video_format_t;

    struct atomic_rc_t {
        std::atomic_uintptr_t refs;
    };

    struct video_palette_t {
        int i_entries;                                  /**< number of in-use palette entries */
        uint8_t palette[VIDEO_PALETTE_COLORS_MAX][4];   /**< 4-byte RGBA/YUVA palette */
    };

    struct viewpoint_t {
        float yaw;   /* yaw in degrees */
        float pitch; /* pitch in degrees */
        float roll;  /* roll in degrees */
        float fov;   /* field of view in degrees */
    };



     struct ancillary
     {
         atomic_rc_t rc;

         uint32 id;
         void* data;
         ancillary_free_cb free_cb;
     };

     struct plugin_t
     {
         struct plugin_t* next;
         module_t* module;
         unsigned modules_count;

         const char* textdomain; /**< gettext domain (or NULL) */

         /**
          * Variables set by the module to store its config options
          */
         struct
         {
             struct param* params; /**< Table of configuration items */
             size_t size; /**< Total count of all items */
             size_t count; /**< Count of real options (excludes hints) */
             size_t booleans; /**< Count of options that are of boolean type */
         } conf;
     };

     struct module_t
     {
         plugin_t* plugin; /**< Plug-in/library containing the module */
         module_t* next;

         /** Shortcuts to the module */
         unsigned    i_shortcuts;
         const char** pp_shortcuts;

         /*
          * Variables set by the module to identify itself
          */
         const char* psz_shortname;                              /**< Module name */
         const char* psz_longname;                   /**< Module descriptive name */
         const char* psz_help;        /**< Long help plain string for "special" modules */
         const char* psz_help_html;   /**< Long help HTML string, shown instead of the plain help where it makes sense to render HTML.
                                           Supports only a limited HTML4 subset, see https://doc.qt.io/qt-6/richtext-html-subset.html */

         const char* psz_capability;                              /**< Capability */
         int      i_score;                          /**< Score for the capability */

         /* Callbacks */
         const char* activate_name;
         const char* deactivate_name;
         void* pf_activate;
     }; 
   
     struct extra_languages_t
     {
         char* psz_language;
         char* psz_description;
     };
          
        struct audio_format_t
        {
            fourcc_t i_format;                          /**< audio format fourcc */
            unsigned int i_rate;                              /**< audio sample-rate */

            /* Describes the channels configuration of the samples (ie. number of
            * channels which are available in the buffer, and positions). */
            uint16_t     i_physical_channels;

            /* Describes the chan mode, either set from the input
            * (demux/codec/packetizer) or overridden by the user, used by audio
            * filters. */
            uint16_t     i_chan_mode;

            /* Channel type */
            audio_channel_type_t channel_type;

            /* Optional - for A/52, SPDIF and DTS types : */
            /* Bytes used by one compressed frame, depends on bitrate. */
            unsigned int i_bytes_per_frame;

            /* Number of sampleframes contained in one compressed frame. */
            unsigned int i_frame_length;
            /* Please note that it may be completely arbitrary - buffers are not
            * obliged to contain a integral number of so-called "frames". It's
            * just here for the division :
            * buffer_size = i_nb_samples * i_bytes_per_frame / i_frame_length
            */

            /* FIXME ? (used by the codecs) */
            unsigned     i_bitspersample;
            unsigned     i_blockalign;
            uint8_t      i_channels; /* must be <=32 */
        };

        struct audio_replay_gain_t
        {
            /* true if we have the peak value */
            bool pb_peak[AUDIO_REPLAY_GAIN_MAX];
            /* peak value where 1.0 means full sample value */
            float      pf_peak[AUDIO_REPLAY_GAIN_MAX];

            /* true if we have the gain value */
            bool pb_gain[AUDIO_REPLAY_GAIN_MAX];
            /* gain value in dB */
            float      pf_gain[AUDIO_REPLAY_GAIN_MAX];
        } ;

        struct subs_format_t
        {
            /* the character encoding of the text of the subtitle.
             * all gettext recognized shorts can be used */
            char* psz_encoding;


            int  i_x_origin; /**< x coordinate of the subtitle. 0 = left */
            int  i_y_origin; /**< y coordinate of the subtitle. 0 = top */

            struct
            {
                /* the width of the original movie the spu was extracted from */
                unsigned i_original_frame_width;
                /* the height of the original movie the spu was extracted from */
                unsigned i_original_frame_height;

                /*  */
                uint32_t palette[VIDEO_PALETTE_CLUT_COUNT]; /* CLUT Palette AYVU */
                bool b_palette;
            } spu;

            struct
            {
                int i_id;
            } dvb;
            struct
            {
                uint8_t i_magazine; /* magazine value (3 bits), > 8 for any */
                uint8_t i_page;     /* BCD packet address value (4+4 bits) */
            } teletext;
            struct
            {
                uint8_t i_channel;
                /* Reorder depth of transport video, -1 for no reordering */
                int i_reorder_depth;
            } cc;
        };

        struct video_format_t
        {
            fourcc_t i_chroma;                               /**< picture chroma */

            unsigned int i_width;                                 /**< picture width */
            unsigned int i_height;                               /**< picture height */

            unsigned int i_x_offset;               /**< start offset of visible area */
            unsigned int i_y_offset;               /**< start offset of visible area */

            unsigned int i_visible_width;                 /**< width of visible area */
            unsigned int i_visible_height;               /**< height of visible area */

            unsigned int i_sar_num;                   /**< sample/pixel aspect ratio */
            unsigned int i_sar_den;

            unsigned int i_frame_rate;                     /**< frame rate numerator */
            unsigned int i_frame_rate_base;              /**< frame rate denominator */

            video_palette_t* p_palette;              /**< video palette from demuxer */
            video_orientation_t orientation;                /**< picture orientation */

            video_color_primaries_t primaries;                  /**< color primaries */

            video_transfer_func_t transfer;                   /**< transfer function */
            video_color_space_t space;                        /**< YCbCr color space */

            video_color_range_t color_range;            /**< 0-255 instead of 16-235 */
            video_chroma_location_t chroma_location;      /**< YCbCr chroma location */

            video_multiview_mode_t multiview_mode;        /** Multiview mode, 2D, 3D */
            bool b_multiview_right_eye_first;   /** Multiview left or right eye first*/

            video_projection_mode_t projection_mode;            /**< projection mode */
            viewpoint_t pose;
            struct {
                /* similar to SMPTE ST 2086 mastering display color volume */
                uint16_t primaries[3 * 2]; /* G,B,R / x,y */
                uint16_t white_point[2]; /* x,y */

                uint32_t max_luminance;
                uint32_t min_luminance;
            } mastering;
            struct {
                /* similar to CTA-861.3 content light level */
                uint16_t MaxCLL;  /* max content light level */
                uint16_t MaxFALL; /* max frame average light level */
            } lighting;
            struct {
                uint8_t version_major;
                uint8_t version_minor;

                unsigned profile : 7;

                unsigned level : 6;
                unsigned rpu_present : 1;

                unsigned el_present : 1;
                unsigned bl_present : 1;
            } dovi;
            uint32_t i_cubemap_padding; /**< padding in pixels of the cube map faces */
        };

     struct es_format_t
     {
         enum es_format_category_e i_cat;    /**< ES category */
         fourcc_t    i_codec;            /**< FOURCC value as used in vlc */
         fourcc_t    i_original_fourcc;  /**< original FOURCC from the container */

         int             i_id;       /**< es identifier, where means
                                         -1: let the core mark the right id
                                         >=0: valid id */
         int             i_group;    /**< group identifier, where means:
                                         -1 : standalone
                                         >= 0 then a "group" (program) is created
                                             for each value */
         int             i_priority; /**< priority, where means:
                                         -2 : mean not selectable by the users
                                         -1 : mean not selected by default even
                                              when no other stream
                                         >=0: priority */

         char* psz_language;        /**< human-readable language name */
         char* psz_description;     /**< human-readable description of language */
         unsigned        i_extra_languages;    /**< length in bytes of extra language data pointer */
         extra_languages_t* p_extra_languages; /**< extra language data needed by some decoders */

         union {
             struct {
                 audio_format_t  audio;    /**< description of audio format */
                 audio_replay_gain_t audio_replay_gain; /**< audio replay gain information */
             };
             video_format_t video;     /**< description of video format */
             subs_format_t  subs;      /**< description of subtitle format */
         };

         unsigned int   i_bitrate; /**< bitrate of this ES */
         int      i_profile;       /**< codec specific information (like real audio flavor, mpeg audio layer, h264 profile ...) */
         int      i_level;         /**< codec specific information: indicates maximum restrictions on the stream (resolution, bitrate, codec features ...) */

         bool     b_packetized;  /**< whether the data is packetized (ie. not truncated) */
         int     i_extra;        /**< length in bytes of extra data pointer */
         void* p_extra;       /**< extra data needed by some decoders or muxers */

     };

     struct config_chain_t
     {
         config_chain_t* p_next;     /**< Pointer on the next config_chain_t element */

         char* psz_name;      /**< Option name */
         char* psz_value;     /**< Option value */
     };

     struct decoder_device_operations
     {
         void (*close)(struct decoder_device*);
     };

     typedef struct decoder_device
     {
         const struct decoder_device_operations* ops;

         /** Private context that could be used by the "decoder device" module
          * implementation */
         void* sys;

         /** Must be set from the "decoder device" module open entry point */
         enum decoder_device_type type;

         /**
          * Could be set from the "decoder device" module open entry point and will
          * be used by hardware decoder modules.
          *
          * The type of pointer will depend of the type:
          * VAAPI: VADisplay
          * VDPAU: vdp_t *
          * DXVA2: d3d9_decoder_device_t*
          * D3D11VA: d3d11_decoder_device_t*
          * VIDEOTOOLBOX: NULL
          * AWindow: android AWindowHandler*
          * NVDEC: decoder_device_nvdec_t*
          * MMAL: MMAL_PORT_T*
          */
         void* opaque;
     } decoder_device;

     struct video_context_operations
     {
         void (*destroy)(void* priv);
     };

     struct fourcc_desc {
         unsigned char alias[4];
         const char* desc;
     };

     struct fourcc_mapping {
         unsigned char alias[4];
         uint32_t fourcc;
     };


     struct video_context
     {
         atomic_rc_t    rc;
         decoder_device* device;
         const struct video_context_operations* ops;
         enum video_context_type private_type;
         size_t private_size;
         uint8_t _private[];
     };

     struct input_attachment_t
     {
         char* psz_name;
         char* psz_mime;
         char* psz_description;

         size_t i_data;
         void* p_data;
     };

     struct filter_t;
     struct filter_owner_t
     {
         union
         {
             const struct filter_video_callbacks* video;
             const struct filter_audio_callbacks* audio;
             const struct filter_subpicture_callbacks* sub;
         };

         /* Input attachments
          * XXX use filter_GetInputAttachments */
         int (*pf_get_attachments)(filter_t*, input_attachment_t***, int*);

         void* sys;
     };

       struct filter_t
     {
         module_t* p_module;
         void* p_sys;

         /* Input format */
         es_format_t         fmt_in;
         video_context* vctx_in;  // video filter, set by owner

         /* Output format of filter */
         es_format_t         fmt_out;
         video_context* vctx_out; // video filter, handled by the filter
         bool                b_allow_fmt_out_change;

         /* Name of the "video filter" shortcut that is requested, can be NULL */
         const char* psz_name;
         /* Filter configuration */
         const config_chain_t* p_cfg;

         /* Implementation of filter API */
         const struct filter_operations* ops;

         /** Private structure for the owner of the filter */
         filter_owner_t      owner;
     };

     struct picture_context_t
    {
        void (*destroy)(struct picture_context_t*);
        struct picture_context_t* (*copy)(struct picture_context_t*);
        struct video_context* vctx;
    };

    struct plane_t {
        uint8_t* p_pixels;                        /**< Start of the plane's data */

        /* Variables used for fast memcpy operations */
        int i_lines;           /**< Number of lines, including margins */
        int i_pitch;           /**< Number of bytes in a line, including margins */

        /** Size of a macropixel, defaults to 1 */
        int i_pixel_pitch;

        /* Variables used for pictures with margins */
        int i_visible_lines;            /**< How many visible lines are there? */
        int i_visible_pitch;            /**< How many bytes for visible pixels are there? */

    };

     struct picture_t {
        /**
         * The properties of the picture
         */
        video_format_t format;

        plane_t         p[PICTURE_PLANE_MAX];     /**< description of the planes */
        int             i_planes;                /**< number of allocated planes */

        /** \name Picture management properties
         * These properties can be modified using the video output thread API,
         * but should never be written directly */
         /**@{*/
        Time::time      date;                                  /**< display date */
        bool            b_force;
        bool            b_still;
        /**@}*/

        /** \name Picture dynamic properties
         * Those properties can be changed by the decoder
         * @{
         */
        bool            b_progressive;          /**< is it a progressive frame? */
        bool            b_top_field_first;             /**< which field is first */

        bool            b_multiview_left_eye; /**< left eye or right eye in multiview */

        unsigned int    i_nb_fields;                  /**< number of displayed fields */
        picture_context_t* context;      /**< video format-specific data pointer */
        /**@}*/

        /** Private data - the video output plugin might want to put stuff here to
         * keep track of the picture */
        void* p_sys;

        /** Next picture in a FIFO a pictures */
        struct picture_t* p_next;

        atomic_rc_t refs;
    };

    struct rational_t {
        unsigned num, den;
    };

     struct chroma_description_t {
        fourcc_t fcc;

        unsigned plane_count;

        struct {
            rational_t w;
            rational_t h;
        } p[4];

        unsigned pixel_size;        /* Number of bytes per pixel for a plane */
        unsigned pixel_bits;        /* Number of bits actually used bits per pixel for a plane */
    } ;

    struct filter_sys_t {
        SwsFilter* p_filter;
        int i_sws_flags;

        video_format_t fmt_in;
        video_format_t fmt_out;

        const chroma_description_t* desc_in;
        const chroma_description_t* desc_out;

        struct SwsContext* ctx;
        struct SwsContext* ctxA;

        picture_t* p_src_a;
        picture_t* p_dst_a;

        int i_extend_factor;

        picture_t* p_src_e;
        picture_t* p_dst_e;

        bool b_add_a;
        bool b_copy;

        bool b_swap_uvi;
        bool b_swap_uvo;
    } ;

    struct ScalerConfiguration {
        enum AVPixelFormat i_fmti;
        enum AVPixelFormat i_fmto;

        bool b_has_a;
        bool b_add_a;

        int  i_sws_flags;
        bool b_copy;

        bool b_swap_uvi;
        bool b_swap_uvo;
    };

    struct picture_priv_t
    {
        picture_t picture;
        struct
        {
            void (*destroy)(picture_t*);
            void* opaque;
        } gc;

        /** Private ancillary struct. Don't use it directly, but use it via
         * picture_AttachAncillary() and picture_GetAncillary(). */
        struct ancillary** ancillaries;
    };


    struct decoder_device_priv
    {
        struct decoder_device device;
        atomic_rc_t rc;
    };

    static const struct fourcc_desc desc_audio[] = {
 {{'a','a','c',' '}, "AAC audio"},
  {{'m','p','4','a'}, "MP4A audio"},
  {{'m','p','3',' '}, "MP3 audio"},
  {{'a','c','3',' '}, "AC3 audio"}
    };
    static const struct fourcc_mapping mapping_audio[] = {
      {{'m','p','4','a'},{'a'}},
       {{'m','p','3',' '},{'m'}}
    };

    // video format
    static const struct fourcc_desc desc_video[] = {
      {{'h','2','6','4'}, "H264 codec"},
      {{'x','2','6','4'}, "X264 codec"},
       {{'m','p','e','g'}, "MPEG codec"},
      {{'a','v','c','1'}, "AVC1 codec"}
    };
    static const struct fourcc_mapping mapping_video[] = {
     {{'x','2','6','4'},{'h'}}
    };

    // spu format
    static const struct fourcc_desc desc_spu[] = {
      {{'s','r','t',' '}, "srt sub"},
      {{'s','u','b',' '}, "sub sub"},
       {{'t','x','t',' '}, "txt sub"},
    };
    static const struct fourcc_mapping mapping_spu[] = {
        {}
    };

    struct picture_buffer_t
    {
        int fd;
        void* base;
        size_t size;
        off_t offset;
    };

    struct picture_priv_buffer_t {
        picture_priv_t   priv;
        picture_buffer_t res;
    };

    struct picture_resource_t
    {
        void* p_sys;
        void (*pf_destroy)(picture_t*);

        /* Plane resources
         * XXX all fields MUST be set to the right value.
         */
        struct
        {
            uint8_t* p_pixels;  /**< Start of the plane's data */
            int i_lines;        /**< Number of lines, including margins */
            int i_pitch;        /**< Number of bytes in a line, including margins */
        } p[PICTURE_PLANE_MAX];

    };
} // namespace FFmpeg