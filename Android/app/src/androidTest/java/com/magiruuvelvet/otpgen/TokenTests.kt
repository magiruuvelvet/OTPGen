package com.magiruuvelvet.otpgen

import com.magiruuvelvet.libotpgen.OTPToken;
import com.magiruuvelvet.libotpgen.TOTPToken;
import com.magiruuvelvet.libotpgen.HOTPToken;
import com.magiruuvelvet.libotpgen.SteamToken;
import com.magiruuvelvet.libotpgen.AuthyToken;
import com.magiruuvelvet.libotpgen.TokenStore;

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

    @Test
    fun test_tokenstore()
    {
        val tokenStore = TokenStore.i();
        assertTrue(tokenStore.empty());
        tokenStore.addToken(TOTPToken("test", "", "secret", 7, 10, 0, OTPToken.ShaAlgorithm.SHA1));
        assertFalse(tokenStore.empty());
        val tokens = tokenStore.tokens();
        assertTrue(tokens.size() == 1L);
        assertTrue(tokenStore.tokenCount() == 1L);

        // OTPTokenPtrList
        // val test = tokens.get(0); <-- native crash
        // assertEquals(test.label(), "test");
        // assertEquals(test.secret(), "secret");
        // assertTrue(test.digits() == 7.toShort());
        // assertTrue(test.period() == 10.toLong());
        // for (i in 0..tokens.size())
        // {
        //     // native crash
        //     assertEquals(tokens[i.toInt()].label(), "test");
        // }

        val token = tokenStore.tokenAt("test");
        assertEquals(token.label(), "test");
        assertEquals(token.secret(), "secret");
        assertTrue(token.digits() == 7.toShort());
        assertTrue(token.period() == 10.toLong());

        val nullToken = tokenStore.tokenAt("test2");
        assertEquals(nullToken, null);
    }
}
