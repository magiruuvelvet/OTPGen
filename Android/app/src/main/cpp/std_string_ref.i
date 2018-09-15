//
// Support for std::string& references
//

// taken from: https://stackoverflow.com/a/11967859

%typemap(jstype) std::string& OUTPUT "String[]"
%typemap(jtype) std::string& OUTPUT "String[]"
%typemap(jni) std::string& OUTPUT "jobjectArray"
%typemap(javain)  std::string& OUTPUT "$javainput"
%typemap(in) std::string& OUTPUT (std::string temp) {
  if (!$input) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
    return $null;
  }
  if (JCALL1(GetArrayLength, jenv, $input) == 0) {
    SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
  }
  $1 = &temp;
}
%typemap(argout) std::string& OUTPUT {
  jstring jvalue = JCALL1(NewStringUTF, jenv, temp$argnum.c_str()); 
  JCALL3(SetObjectArrayElement, jenv, $input, 0, jvalue);
}
