// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.android_webview.shell;

import android.app.Application;
import android.os.Debug;
import android.util.Log;

import org.chromium.android_webview.AwBrowserProcess;
import org.chromium.base.BaseSwitches;
import org.chromium.base.CommandLine;
import org.chromium.content.browser.ResourceExtractor;

public class AwShellApplication extends Application {

    private static final String TAG = "AwShellApplication";
    /** The minimum set of .pak files the test runner needs. */
    private static final String[] MANDATORY_PAKS = {
        "webviewchromium.pak", "en-US.pak", "icudtl.dat"
    };

    @Override
    public void onCreate() {
        super.onCreate();

        AwShellResourceProvider.registerResources(this);

        CommandLine.initFromFile("/data/local/tmp/android-webview-command-line");

        if (CommandLine.getInstance().hasSwitch(BaseSwitches.WAIT_FOR_JAVA_DEBUGGER)) {
            Log.e(TAG, "Waiting for Java debugger to connect...");
            Debug.waitForDebugger();
            Log.e(TAG, "Java debugger connected. Resuming execution.");
        }

        ResourceExtractor.setMandatoryPaksToExtract(MANDATORY_PAKS);
        ResourceExtractor.setExtractImplicitLocaleForTesting(false);
        AwBrowserProcess.loadLibrary();
    }
}
