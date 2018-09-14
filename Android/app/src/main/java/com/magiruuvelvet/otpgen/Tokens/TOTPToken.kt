package com.magiruuvelvet.otpgen.Tokens

open class TOTPToken : OTPToken()
{
    init
    {
        _digits = 6;
        _period = 30;
        _algorithm = Companion.ShaAlgorithm.SHA1;
    }

    // remove counter from TOTP
    override fun setCounter(counter: Int) {}

    override val _type: Companion.Type = Companion.Type.TOTP;

    override fun generateToken(): String
    {
        return OTPGen.computeTOTP(_secret, _digits, _period, _algorithm);
    }
}
