package com.magiruuvelvet.otpgen.Tokens

abstract class OTPToken
{
    companion object
    {
        // enum of token types, values must match with the C++ header (OTPToken.hpp)
        enum class Type(val type: Int)
        {
            None(0),
            TOTP(1),
            HOTP(2),
            Steam(3),
            Authy(4),
        }

        // enum of SHA algorithms, values must match with the C++ header (OTPToken.hpp)
        enum class ShaAlgorithm(val value: Int)
        {
            Invalid(0),
            SHA1(1),
            SHA256(2),
            SHA512(3),
        }
    }

    protected abstract val _type: Type; // must not be changed, must be initialized in subclass
    protected var _label: String = String();
    protected var _icon: ByteArray = ByteArray(0);

    protected var _secret: String = String();
    protected var _digits: Int = 0;
    protected var _period: Int = 0;
    protected var _counter: Int = 0;
    protected var _algorithm: ShaAlgorithm = ShaAlgorithm.Invalid;

    fun setLabel(label: String)
    {
        _label = label;
    }

    fun label(): String
    {
        return _label;
    }

    fun setIcon(icon: ByteArray)
    {
        _icon = icon;
    }

    fun icon(): ByteArray
    {
        return _icon;
    }

    fun setSecret(secret: String)
    {
        _secret = secret;
    }

    fun secret(): String
    {
        return _secret;
    }

    fun setDigits(digits: Int)
    {
        _digits = digits;
    }

    fun digits(): Int
    {
        return _digits;
    }

    open fun setPeriod(period: Int)
    {
        _period = period;
    }

    fun period(): Int
    {
        return _period;
    }

    open fun setCounter(counter: Int)
    {
        _counter = counter;
    }

    fun counter(): Int
    {
        return _counter;
    }

    open fun setAlgorithm(algo: ShaAlgorithm)
    {
        _algorithm = algo;
    }

    fun algorithm(): ShaAlgorithm
    {
        return _algorithm;
    }

    abstract fun generateToken(): String;
}
