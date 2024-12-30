#pragma once

#include "../../../core/Types.h"
#include "../../../core/Time.h"

#include <stdatomic.h>
#include <Windows.h>

#ifdef __STDC_NO_ATOMICS__ 
#undef __STDC_NO_ATOMICS__ 
#endif // __STDC_NO_ATOMICS__ 


extern "C" {
    #include <libswscale/swscale.h>
}


namespace FFmpeg {

    struct module_t;
    struct plagin_t;
    struct video_format_t;

    union value_t
    {
        int64_t         i_int;
        bool            b_bool;
        float           f_float;
        char* psz_string;
        void* p_address;
        struct { int32_t x; int32_t y; } coords;

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

     union module_value_t
     {
         char* psz;
         int64_t     i;
         float       f;
     };
   
     struct module_config_t
     {
         uint8_t     i_type; /**< Configuration type */

         const char* psz_type; /**< Configuration subtype */
         const char* psz_name; /**< Option name */
         const char* psz_text; /**< Short comment on the configuration option */
         const char* psz_longtext; /**< Long comment on the configuration option */

         module_value_t value; /**< Current value */
         module_value_t orig; /**< Default value */
         module_value_t min; /**< Minimum value (for scalars only) */
         module_value_t max; /**< Maximum value (for scalars only) */

         /* Values list */
         uint16_t list_count; /**< Choices count */
         union
         {
             const char** psz; /**< Table of possible string choices */
             const int* i; /**< Table of possible integer choices */
         } list; /**< Possible choices */
         const char** list_text; /**< Human-readable names for list values */
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

    enum es_format_category_e {
        UNKNOWN_ES = 0x00,
        VIDEO_ES,
        AUDIO_ES,
        SPU_ES,
        DATA_ES,
    };

    enum audio_channel_type_t {
        AUDIO_CHANNEL_TYPE_BITMAP,
        AUDIO_CHANNEL_TYPE_AMBISONICS,
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

     struct variable_ops_t
     {
         int  (*pf_cmp) (value_t, value_t);
         void (*pf_dup) (value_t*);
         void (*pf_free) (value_t*);
     };

     typedef int (*callback_t) (object_t*,      /* variable's object */
         char const*,            /* variable name */
         value_t,                 /* old value */
         value_t,                 /* new value */
         void*);                /* callback data */

     /*****************************************************************************
      * List callbacks: called when elements are added/removed from the list
      *****************************************************************************/
     typedef int (*list_callback_t) (object_t*,      /* variable's object */
         char const*,            /* variable name */
         int,                  /* VAR_* action */
         value_t*,      /* new/deleted value  */
         void*);                 /* callback data */


     struct callback_entry_t
     {
         struct callback_entry_t* next;
         union
         {
             callback_t       pf_value_callback;
             list_callback_t  pf_list_callback;
             void* p_callback;
         };
         void* p_data;
     };

     struct variable_t
     {
         char* psz_name; /**< The variable unique name (must be first) */

         /** The variable's exported value */
         value_t  val;

         /** The variable display name, mainly for use by the interfaces */
         char* psz_text;

         const variable_ops_t* ops;

         int          i_type;   /**< The type of the variable */
         unsigned     i_usage;  /**< Reference count */

         /** If the variable has min/max/step values */
         value_t  min, max, step;

         /** List of choices */
         value_t* choices;
         /** List of friendly names for the choices */
         char** choices_text;
         size_t       choices_count;

         /** Set to TRUE if the variable is in a callback */
         bool   b_incallback;

         /** Registered value callbacks */
         callback_entry_t* value_callbacks;
         /** Registered list callbacks */
         callback_entry_t* list_callbacks;

         Threads::cond_t   wait;
     };

     struct video_context
     {
         Threads::atomic_rc_t    rc;
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

 
     struct rcu_generation {
         std::atomic_uintptr_t readers;
         std::atomic_uint writer;
     };

     struct rcu_thread {
         rcu_generation* generation;
         uintptr_t recursion;
     };

     struct param {
         union {
             /*_Atomic*/ std::atomic<int64_t> i; /**< Current value (if integer or boolean) */
             /*_Atomic*/ std::atomic<float> f; /**< Current value (if floating point) */
             std::atomic<char*> /*_Atomic*/ str; /**< Current value (if character string) */
         } value;

         struct 
             

         plugin_t* owner;
         unsigned char shortname; /**< Optional short option name */
         unsigned internal:1; /**< Hidden from preferences and help */
         unsigned unsaved : 1; /**< Not stored in persistent configuration */
         unsigned safe : 1; /**< Safe for untrusted provisioning (playlists) */
         unsigned obsolete : 1; /**< Ignored for backward compatibility */
         struct module_config_t item;
     };

    struct decoder_device_priv
    {
        struct decoder_device device;
        Threads::atomic_rc_t rc;
    };
} // namespace FFmpeg