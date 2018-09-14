package com.magiruuvelvet.otpgen.Tokens

class AuthyToken : TOTPToken()
{
    init
    {
        _digits = 7;
        _period = 10;
        _algorithm = Companion.ShaAlgorithm.SHA1;
    }

    override val _type: Companion.Type = Companion.Type.Authy;

    override fun setCounter(counter: Int) {}
    override fun setPeriod(period: Int) {}
    override fun setDigits(digits: Int) {}
    override fun setAlgorithm(algo: Companion.ShaAlgorithm) {}
}
