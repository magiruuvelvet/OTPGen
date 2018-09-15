package com.magiruuvelvet.otpgen

import com.magiruuvelvet.libotpgen.TOTPToken;
import com.magiruuvelvet.libotpgen.HOTPToken;
import com.magiruuvelvet.libotpgen.SteamToken;
import com.magiruuvelvet.libotpgen.AuthyToken;

import android.support.test.InstrumentationRegistry
import android.support.test.runner.AndroidJUnit4

import org.junit.Test
import org.junit.runner.RunWith

import org.junit.Assert.*

@RunWith(AndroidJUnit4::class)
class TokenTests
{
    companion object
    {
        init
        {
            System.loadLibrary("otpgen-jni-bindings");
        }
    }

    @Test
    fun test_totp_class()
    {
        val token = TOTPToken();
        token.setSecret("XYZA123456KDDK83D");
        assertEquals(token.secret(), "XYZA123456KDDK83D");
        assertTrue(token.generateToken().length == token.digits().toInt());
        assertTrue(token.digits() == 6.toShort());
        assertTrue(token.period() == 30.toLong());
    }

    @Test
    fun test_steam_class()
    {
        val token = SteamToken();
        token.setSecret("XYZA123456KDDK83D");
        assertTrue(token.generateToken().length == token.digits().toInt());
        assertTrue(token.digits() == 5.toShort());
        assertTrue(token.period() == 30.toLong());
    }

    @Test
    fun test_authy_class()
    {
        val token = AuthyToken();
        token.setSecret("XYZA123456KDDK83D")
        assertTrue(token.generateToken().length == token.digits().toInt());
        assertTrue(token.digits() == 7.toShort());
        assertTrue(token.period() == 10.toLong());
    }
}
