package com.magiruuvelvet.otpgen

import com.magiruuvelvet.otpgen.Tokens.TOTPToken
import com.magiruuvelvet.otpgen.Tokens.SteamToken
import com.magiruuvelvet.otpgen.Tokens.AuthyToken

import android.support.test.InstrumentationRegistry
import android.support.test.runner.AndroidJUnit4

import org.junit.Test
import org.junit.runner.RunWith

import org.junit.Assert.*

@RunWith(AndroidJUnit4::class)
class TokenTests
{
    @Test
    fun test_totp_class()
    {
        val token = TOTPToken();
        token.setSecret("XYZA123456KDDK83D");
        assertEquals(token.secret(), "XYZA123456KDDK83D");
        assertEquals(token.generateToken().length, token.digits());
        assertEquals(token.digits(), 6);
        assertEquals(token.period(), 30);
    }

    @Test
    fun test_steam_class()
    {
        val token = SteamToken();
        token.setSecret("XYZA123456KDDK83D");
        assertEquals(token.generateToken().length, token.digits());
        assertEquals(token.digits(), 5);
        assertEquals(token.period(), 30);
    }

    @Test
    fun test_authy_class()
    {
        val token = AuthyToken();
        token.setSecret("XYZA123456KDDK83D")
        assertEquals(token.generateToken().length, token.digits());
        assertEquals(token.digits(), 7);
        assertEquals(token.period(), 10);
    }
}
