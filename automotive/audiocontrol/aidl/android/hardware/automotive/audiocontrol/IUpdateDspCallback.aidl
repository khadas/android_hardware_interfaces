package android.hardware.automotive.audiocontrol;

/**
 * Interface definition for update dsp progress callback
 */
@VintfStability
oneway interface IUpdateDspCallback {
    /**
     * dsp progress updating
     *
     */
    void onDspUpadateProgessChanged(int state, float progress);
}
