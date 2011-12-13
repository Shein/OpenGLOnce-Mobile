package com.loft.ndkgl;

import android.content.Context;
import android.view.MotionEvent;

public class OGL1SurfaceView extends OGLSurfaceView {
    public OGL1SurfaceView(Context context) {
        super(context);
        mRenderer = new OGLRenderer();
        setRenderer(mRenderer);
    }

    public boolean onTouchEvent(final MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {

        }
        return true;
    }

    OGLRenderer mRenderer;

}