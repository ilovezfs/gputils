; tests for bug #423850 Hang on bad #define directive

     list p=16f84

#define foo foo

     movlw foo

     end
