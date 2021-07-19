//
// Created by Roman Shterenzon on 2/8/20.
//

#ifndef _TINYMIDIPLAYER_H
#define _TINYMIDIPLAYER_H

struct tsf;

class AAssetManager;

class TinyMidiPlayer {
public:
    TinyMidiPlayer(unsigned sampleRate, const char *fontName, AAssetManager *assetManager);

    virtual ~TinyMidiPlayer();

    bool process(short *buf, unsigned len);

    bool noteOn(int note, int velocity = 80);

    bool noteOff(int note);

    bool isEnabled() const {
        return enabled;
    }

    void setEnabled(bool flag) {
        enabled = flag;
    }

private:
    bool enabled = true;
    tsf *tsfHandle;
};


#endif //_TINYMIDIPLAYER_H
