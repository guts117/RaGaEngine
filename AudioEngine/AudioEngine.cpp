#include <vector>

#include <stdio.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#include "printing.h"

// TODO(mdizdar): stop mallocing everywhere

struct AudioClip {
    b8 loop = 1;
    b8 playing = 0;
    u8 volume; // do we need more range?
    u16 channels;
    u16 bits_per_sample;
    u32 sample_rate; // Hz
    u32 duration; // seconds
    u64 cursor = 0;
    u64 samples_number;
    u64 data_size;
    u8 *data;
};

struct AudioSource {
    AudioClip *clip = nullptr;

    AudioSource() = default;

    AudioClip *readWAV(char *filename) {
        FILE *fp = fopen(filename, "rb");
        if (!fp) {
            error("File `%s` does not exist", filename);
        }

        // header
        char buf_[44];
        char *buf = buf_;
        if (size_t read_cnt = fread(buf, 1, 44, fp); read_cnt != 44) {
            error("File `%s` is not a valid WAV file\n", filename);
        }
        
        if (buf[0] != 'R' || buf[1] != 'I' || buf[2] != 'F' || buf[3] != 'F') {
            error("File `%s` is not a valid WAV file\n", filename);
        }
        buf += 4;

        u32 fsize = *(u32 *)buf;
        buf += 4;

        if (buf[0] != 'W' || buf[1] != 'A' || buf[2] != 'V' || buf[3] != 'E') {
            error("File `%s` is not a valid WAV file\n", filename);
        }
        buf += 4;

        if (buf[0] != 'f' || buf[1] != 'm' || buf[2] != 't' || buf[3] != ' ') {
            error("File `%s` is not a valid WAV file\n", filename);
        }
        buf += 4;

        u32 format_data_length = *(u32 *)buf;
        buf += 4;

        u16 format_type = *(u16 *)buf;
        buf += 2;

        u16 channels = *(u16 *)buf;
        buf += 2;

        u32 sample_rate = *(u32 *)buf;
        buf += 4;

        u32 bytes_per_second = *(u32 *)buf;
        buf += 4;

        u16 something = *(u16 *)buf; // no idea what this is
        buf += 2;

        u16 bits_per_sample = *(u16 *)buf;
        buf += 2;

        if (buf[0] != 'd' || buf[1] != 'a' || buf[2] != 't' || buf[3] != 'a') {
            error("File `%s` is not a valid WAV file\n", filename);
        }
        buf += 4;

        u32 data_section_size = *(u32 *)buf;

        u8 *data = (u8 *)malloc(data_section_size); // don't malloc here, arenas are better
        if (size_t read_cnt = fread(data, 1, data_section_size, fp); read_cnt != data_section_size) {
            error("File `%s` is not a valid WAV file\n", filename);
        }

        fclose(fp);

        AudioClip *clip = (AudioClip *)malloc(sizeof(AudioClip));
        clip->bits_per_sample = bits_per_sample;
        clip->channels = channels;
        clip->duration = data_section_size / bytes_per_second;
        clip->sample_rate = sample_rate;
        clip->volume = 255;
        clip->samples_number = data_section_size / (bits_per_sample / 8);
        clip->data_size = data_section_size;
        clip->data = data;
        clip->cursor = 0;
        clip->loop = false;
        clip->playing = true;

        printf("bits_per_sample %u\n", bits_per_sample);
        printf("channels %u\n", channels);
        printf("duration %us\n", clip->duration);
        printf("sample_rate %u\n", sample_rate);
        printf("volume %u\n", clip->volume);
        printf("number of samples %lu\n", clip->samples_number);

        return clip;
    }

    const u8 *update(u8 *buffer, u32 ms) {
        for (u64 i = 0, moveby = ms * clip->sample_rate * clip->bits_per_sample / 8 / 1000;
             i < moveby;
             ++i) {
            if (clip->cursor >= clip->data_size) {
                if (clip->loop) {
                    clip->cursor = 0;
                } else {
                    clip->playing = false;
                }
            }
            if (clip->playing) {
                buffer[i] = clip->data[clip->cursor];
                clip->cursor += 1;
            } else {
                buffer[i] = 0;
            }
        }
        return buffer;
    }
};

struct AudioListener {
    pa_simple *audio_handle;

    AudioListener() {
        static const pa_sample_spec ss = {
            .format = PA_SAMPLE_S16LE,
            .rate = 16000,
            .channels = 2
        };
        int err;

        audio_handle = pa_simple_new(NULL, "NK_AudioEngine", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &err);
        if (!audio_handle) {
            error("pa_simple_new() failed: %s\n", pa_strerror(err));
        }
    }

    ~AudioListener() {
        int err;
        if (pa_simple_drain(audio_handle, &err) < 0) {
            error("pa_simple_drain() failed: %s\n", pa_strerror(err));
        }
        if (audio_handle) {
            pa_simple_free(audio_handle);
        }
    }

    void update(std::vector<AudioSource> &sources, u32 ms) {
        int err;
        const u64 size = ms * sources[0].clip->sample_rate * sources[0].clip->bits_per_sample / 8 / 1000;
        u8 buffer[size];
        if (pa_simple_write(audio_handle, sources[0].update(buffer, ms), (size_t)(size), &err) < 0) {
            error("pa_simple_write() failed: %s\n", pa_strerror(err));
        }
    }
};

int main(int argc, char **argv) {
    AudioSource source;
    source.clip = source.readWAV(argv[1]);

    std::vector<AudioSource> sources;
    sources.emplace_back(source);

    AudioListener listener;

    for (;;) {
        listener.update(sources, 16);
    }

    return 0;
}