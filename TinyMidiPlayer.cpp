//
// Created by Roman Shterenzon on 2/8/20.
//

#include <android/asset_manager.h>
#include <memory>
#include "TinyMidiPlayer.h"

#define TSF_IMPLEMENTATION
#define TSF_NO_STDIO
#define TSF_STATIC

#include "tsf.h"

class AAsetStream {
public:
    AAsetStream(AAssetManager *assetManager, const char *name) {
        asset = AAssetManager_open(assetManager, name, AASSET_MODE_RANDOM);
    }

    ~AAsetStream() {
        AAsset_close(asset);
    }

    static int readImpl(void *data, void *ptr, unsigned int size) {
        return static_cast<AAsetStream *>(data)->read(ptr, size);
    }

    static int skipImpl(void *data, unsigned int count) {
        return static_cast<AAsetStream *>(data)->skip(count);
    }

    int read(void *destBuffer, unsigned int size) {
        return AAsset_read(asset, destBuffer, size);
    }

    int skip(unsigned int count) {
        return AAsset_seek(asset, count, SEEK_CUR);
    }

private:
    AAsset *asset;
};

using namespace std;

TinyMidiPlayer::TinyMidiPlayer(unsigned sampleRate, const char *fontName,
                               AAssetManager *assetManager) {
    unique_ptr<AAsetStream> stream = make_unique<AAsetStream>(assetManager, fontName);
    struct tsf_stream ts = {stream.get(), AAsetStream::readImpl, AAsetStream::skipImpl};
    tsfHandle = tsf_load(&ts);
    tsf_set_output(tsfHandle, TSF_STEREO_INTERLEAVED, sampleRate);
    tsf_set_max_voices(tsfHandle, 64);
}

TinyMidiPlayer::~TinyMidiPlayer() {
    tsf_close(tsfHandle);
    tsfHandle = nullptr;
}

bool TinyMidiPlayer::process(short *buf, unsigned len) {
    if (!enabled) {
        return false;
    }
    tsf_render_short(tsfHandle, buf, len, 0);
    for (int i = 0; i < len * 2; i++) {
        // use 10 as threshold, since it often generates small numbers as silence, e.g. 1
        if (buf[i] > 10) {
            return true;
        }
    }
    return false;
}

bool TinyMidiPlayer::noteOn(int note, int velocity) {
    if (!enabled) {
        return false;
    }
    tsf_note_on(tsfHandle, 0, note, (float) velocity / 127.f);
    return true;
}

bool TinyMidiPlayer::noteOff(int note) {
    if (!enabled) {
        return false;
    }
    tsf_note_off(tsfHandle, 0, note);
    return true;
}
