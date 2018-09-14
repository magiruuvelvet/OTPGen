package com.magiruuvelvet.otpgen.Tokens

class AuthyToken : TOTPToken()
{
    init
    {
        _digits = 7;
        _period = 10;
        _algorithm = Companion.ShaAlgorithm.SHA1;
    }

    override fun setCounter(counter: Int) {}
    override fun setPeriod(period: Int) {}
    override fun setDigits(digits: Int) {}
    override fun setAlgorithm(algo: Companion.ShaAlgorithm) {}
}
