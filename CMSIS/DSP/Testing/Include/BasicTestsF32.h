#include "Test.h"
#include "Pattern.h"
class BasicTestsF32:public Client::Suite
    {
        public:
            BasicTestsF32(Testing::testID_t id);
            void setUp(Testing::testID_t,std::vector<Testing::param_t>& params,Client::PatternMgr *mgr);
            void tearDown(Testing::testID_t,Client::PatternMgr *mgr);
        private:
            #include "BasicTests_decl.h"
            
            Client::Pattern<float32_t> input1;
            Client::Pattern<float32_t> input2;
            Client::LocalPattern<float32_t> output;
            // Reference patterns are not loaded when we are in dump mode
            Client::RefPattern<float32_t> ref;
    };