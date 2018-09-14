package com.magiruuvelvet.otpgen

import com.magiruuvelvet.otpgen.Tokens.OTPGen
import com.magiruuvelvet.otpgen.Tokens.OTPToken

import android.support.test.InstrumentationRegistry
import android.support.test.runner.AndroidJUnit4

import org.junit.Test
import org.junit.runner.RunWith

import org.junit.Assert.*

@RunWith(AndroidJUnit4::class)
class DeviceUnitTests
{
    @Test
    fun useAppContext()
    {
        // Context of the app under test.
        val appContext = InstrumentationRegistry.getTargetContext();
        assertEquals("com.magiruuvelvet.otpgen", appContext.packageName);
    }

    // exported JNI functions
    private external fun stringFromJNI(): String;
    private external fun testTotpGeneration(secret: String, digits: Int, period: Int, time: Long): String;
    private external fun testHotpGeneration(secret: String, digits: Int, counter: Int): String;
    private external fun testSteamGeneration(secret: String, time: Long): String;

    companion object
    {
        init
        {
            System.loadLibrary("otpgen-jni-bindings");
        }
    }

    @Test
    fun test_native_library()
    {
        assertEquals(stringFromJNI(), "Hello from C++");
    }

    @Test
    fun test_totp_generation()
    {
        var token = testTotpGeneration("XYZA123456KDDK83D", 6, 30, 1536573862);
        assertEquals(token, "122810");

        token = testTotpGeneration("XYZA123456KDDK83D28273", 7, 10, 1536573862);
        assertEquals(token, "8578249");
    }

    @Test
    fun test_hotp_generation()
    {
        val token = testHotpGeneration("XYZA123456KDDK83D", 6, 12);
        assertEquals(token, "534003");
    }

    @Test
    fun test_steam_generation()
    {
        val token = testSteamGeneration("ABC30WAY33X57CCBU3EAXGDDMX35S39M", 1536573862);
        assertEquals(token, "GQTTM");
    }

    @Test
    fun test_otpgen_exceptions()
    {
        try {
            val token = OTPGen.computeTOTP("", 6, 30, OTPToken.Companion.ShaAlgorithm.SHA1);
        } catch (e: Exception) {
            assertEquals(e.message, "OTPGen: Invalid base-32 input!");
            return;
        }
        // exception test failed
        assertEquals(true, false);
    }
}
