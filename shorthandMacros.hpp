/**
 * FORWARD - forward the responsibility to a member object.
 * Example:
 * class Fowarder{
 *     private:
 *     std::string member;
 *     public:
 *     //equivalent to:
 *     //const char* c_str() const { return member.c_str(); }
 *     FORWARD(member, const char*, c_str, const)
 * };
*/
#define FORWARD(OBJ, RET_TYPE, FUNC_NAME, FUNC_POSTFIX) RET_TYPE FUNC_NAME() FUNC_POSTFIX { return OBJ.FUNC_NAME(); }

/**
 * TESTCASES, TESTCASES_END
 * Macro for enclosing test cases, implemented using static constructor.
 * When NDEBUG is set, the test cases will NOT be executed.
 * Pros:
 *    Runs before the main() starts. Can ensure that all test cases are passed before main() starts
 *    Lightweight. It's implemented with a couple of simple macros.
 * Cons:
 *    Cannot be used to test static objects because of "static initialization order fiasco":
 *        http://www.parashift.com/c++-faq-lite/static-init-order.html
 *    Can only have one test case block per class. Not suitable for scaled project.
 * 
 * Example:
 * TESTCASES(Player)
 *     Player player;
 *     player.setPos(20, 0);
 *     player.jump()
 *     delay(20);
 *     assert(player.getPos()==Vec(20, 90));
 * TESTCASES_END(Player)
*/
#ifndef NDEBUG
	#define TESTCASES(CLASS_NAME)\
	namespace{\
		class CLASS_NAME##TestCases{\
			CLASS_NAME##TestCases(){

	#define TESTCASES_END(CLASS_NAME)\
			}\
			static CLASS_NAME##TestCases staticConstructor;\
		};\
		CLASS_NAME##TestCases CLASS_NAME##TestCases::staticConstructor;\
	}
#else
	#define TESTCASES(CLASS_NAME)
	#define TESTCASES_END(CLASS_NAME)
#endif
