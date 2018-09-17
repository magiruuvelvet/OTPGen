package com.magiruuvelvet.otpgen

import com.magiruuvelvet.libotpgen.OTPGen
import com.magiruuvelvet.libotpgen.OTPToken
import com.magiruuvelvet.libotpgen.libotpgen

// import com.magiruuvelvet.otpgen.Utils.ResourceManager
// import com.magiruuvelvet.otpgen.test.R;

import android.support.test.InstrumentationRegistry
import android.support.test.runner.AndroidJUnit4

import org.junit.Test
import org.junit.runner.RunWith

import org.junit.Assert.*

@RunWith(AndroidJUnit4::class)
class DeviceUnitTests
{
    companion object
    {
        init
        {
            System.loadLibrary("otpgen-jni-bindings");
        }
    }

    @Test
    fun useAppContext()
    {
        // Context of the app under test.
        val appContext = InstrumentationRegistry.getTargetContext();
        assertEquals("com.magiruuvelvet.otpgen", appContext.packageName);
    }

    // FIXME: test currently failing, wrong file size
    //   PNG has 626 bytes, but after writing the file to the cache the size
    //   is 852 bytes with random garbage added :/
    // @Test
    // fun test_qr_decode()
    // {
    //     val appContext = InstrumentationRegistry.getTargetContext();
    //
    //     // write test resource to apps cache directory
    //     val valid_png_path = appContext.cacheDir.absolutePath + "/valid.png";
    //     ResourceManager.createFile(valid_png_path, appContext, R.raw.valid_png);
    //
    //     // start QR decode test
    //     val res = libotpgen.QRCode_decode(valid_png_path);
    //     assertEquals(res.getStatus(), true);
    //     assertEquals(res.getData(), "otpauth://totp/Example:alice@google.com?secret=JBSWY3DPEHPK3PXP&issuer=Example");
    // }

    @Test
    fun test_totp_generation()
    {
        var token = OTPGen.computeTOTP(1536573862, "XYZA123456KDDK83D", 6, 30, OTPToken.ShaAlgorithm.SHA1);
        assertEquals(token, "122810");

        token = OTPGen.computeTOTP(1536573862, "XYZA123456KDDK83D28273", 7, 10, OTPToken.ShaAlgorithm.SHA1);
        assertEquals(token, "8578249");
    }

    @Test
    fun test_hotp_generation()
    {
        val token = OTPGen.computeHOTP("XYZA123456KDDK83D", 12, 6, OTPToken.ShaAlgorithm.SHA1);
        assertEquals(token, "534003");
    }

    @Test
    fun test_steam_generation()
    {
        val token = OTPGen.computeSteam(1536573862, "ABC30WAY33X57CCBU3EAXGDDMX35S39M");
        assertEquals(token, "GQTTM");
    }

    @Test
    fun test_otpgen_error()
    {
        val token = OTPGen.computeTOTP("", 6, 30, OTPToken.ShaAlgorithm.SHA1);
        assertEquals(token, "");
    }
}
