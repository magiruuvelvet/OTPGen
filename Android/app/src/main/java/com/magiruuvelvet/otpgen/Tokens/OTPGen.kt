package com.magiruuvelvet.otpgen.Tokens

internal class OTPGen
{
    companion object
    {
        // Load libotpgen-jni-bindings
        init { System.loadLibrary("otpgen-jni-bindings"); }

        // exported JNI symbols
        private external fun _computeTOTP(secret: String, digits: Int, period: Int, algo: Int): String;
        //private external fun _computeTOTP_AtTime(secret: String, digits: Int, period: Int, algo: Int, time: Long): String;
        private external fun _computeHOTP(secret: String, digits: Int, counter: Int, algo: Int): String;
        private external fun _computeSteam(secret: String): String;
        //private external fun _computeSteam_AtTime(secret: String, time: Long): String;

        /**
         * Computes a TOTP token from the given base-32 secret.
         */
        fun computeTOTP(secret: String, digits: Int, period: Int, algo: OTPToken.Companion.ShaAlgorithm): String
        {
            return _computeTOTP(secret, digits, period, algo.value);
        }

        /**
         * Computes a HOTP token from the given base-32 secret.
         */
        fun computeHOTP(secret: String, digits: Int, counter: Int, algo: OTPToken.Companion.ShaAlgorithm): String
        {
            return _computeHOTP(secret, digits, counter, algo.value);
        }

        /**
         * Computes a Steam token from the given base-32 secret.
         */
        fun computeSteam(secret: String): String
        {
            return _computeSteam(secret);
        }
    }
}
