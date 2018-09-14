package com.magiruuvelvet.otpgen.Tokens

class HOTPToken : OTPToken()
{
    init
    {
        _digits = 6;
        _counter = 0;
        _algorithm = Companion.ShaAlgorithm.SHA1;
    }

    override val _type: Companion.Type = Companion.Type.HOTP;

    // remove period from HOTP
    override fun setPeriod(period: Int) {}

    override fun generateToken(): String
    {
        return OTPGen.computeHOTP(_secret, _digits, _counter, _algorithm);
    }
}
