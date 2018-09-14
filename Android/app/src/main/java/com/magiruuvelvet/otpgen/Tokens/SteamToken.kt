package com.magiruuvelvet.otpgen.Tokens

class SteamToken : OTPToken()
{
    init
    {
        _digits = 5;
        _period = 30;
        _algorithm = Companion.ShaAlgorithm.SHA1;
    }

    override fun setCounter(counter: Int) {}
    override fun setPeriod(period: Int) {}
    override fun setDigits(digits: Int) {}
    override fun setAlgorithm(algo: Companion.ShaAlgorithm) {}

    override val _type: Companion.Type = Companion.Type.Steam;

    override fun generateToken(): String
    {
        return OTPGen.computeSteam(_secret);
    }
}
