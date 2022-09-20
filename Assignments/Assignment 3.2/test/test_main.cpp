#include <util/delay.h>
#include <unity.h>

#include <fifo.h>

void test_reset(void){
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.get();
    f.put(3);
    f.reset();

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.is_empty());
}

void test_normal_flow(void)
{
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.get();
    f.put(3);

    // 2-3 Execute and validate
    TEST_ASSERT_EQUAL(2, f.get());
    TEST_ASSERT_EQUAL(3, f.get());

    // 4 Cleanup
}

void test_underflow(void)

{
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.get();
    f.get();

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.is_empty());

    // 4 Cleanup
}

void test_overflow(void){
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.put(3);
    f.put(4);
    f.put(5);
    

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.is_full());
}

void test_overwrite(void){
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.put(3);
    f.put(4);
    f.put(5);
    f.put(6);
    

    // 2-3 Execute and validate
    TEST_ASSERT_EQUAL(2,f.get());
}



void circular_test_reset(void){
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.get();
    f.put(3);
    f.reset();

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.is_empty());
}

void circular_test_normal_flow(void)
{
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.get();
    f.put(3);

    // 2-3 Execute and validate
    TEST_ASSERT_EQUAL(2, f.get());
    TEST_ASSERT_EQUAL(3, f.get());

    // 4 Cleanup
}

void circular_test_underflow(void){
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.get();
    f.get();

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.is_empty());

    // 4 Cleanup
}

void circular_test_overflow(void){
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.put(3);
    f.put(4);
    f.put(5);
    

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.is_full());
}

void circular_test_overwrite(void){
    // 1 Setup
    Fifo f;
    f.put(1);
    f.put(2);
    f.put(3);
    f.put(4);
    f.put(5);
    f.put(6);
    

    // 2-3 Execute and validate
    TEST_ASSERT_EQUAL(2,f.get());
}


void test_circ_normal_flow(void)
{
    // 1 Setup
    Fifo f;
    f.circular_put(1);
    f.circular_put(2);
    f.circular_get();
    f.circular_put(3);

    // 2-3 Execute and validate
    TEST_ASSERT_EQUAL(2, f.circular_get());
    TEST_ASSERT_EQUAL(3, f.circular_get());

    // 4 Cleanup
}

void test_circ_underflow(void)

{
    // 1 Setup
    Fifo f;
    f.circular_put(1);
    f.circular_put(2);
    f.circular_get();
    f.circular_get();
    f.circular_get();

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.circular_is_empty());

    // 4 Cleanup
}

void test_circ_overflow(void)

{
    // 1 Setup
    Fifo f;
    f.circular_put(1);
    f.circular_put(2);
    f.circular_put(3);
    f.circular_put(4);
    f.circular_put(5);
    // f.put(6);
    // f.get();
    // f.get();

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.circular_is_full());

    // 4 Cleanup
}

void test_circ_reset(void)

{
    // 1 Setup
    Fifo f;
    f.circular_reset();
    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.circular_is_empty());

    // 4 Cleanup
}

void test_circ_overwrite(void)

{
    // 1 Setup
    Fifo f;
    f.circular_put(1);
    f.circular_put(2);
    f.circular_put(3);
    f.circular_put(4);
    f.circular_put(5);
    f.circular_put(6);
    f.circular_put(7);
    f.circular_put(8);
    f.circular_put(9);
    f.circular_put(10);
    f.circular_put(11);
    f.circular_put(12);


    // 2-3 Execute and validate
    
    // TEST_ASSERT_EQUAL(4, f.get());
    // TEST_ASSERT_EQUAL(5, f.get());
    // TEST_ASSERT_EQUAL(6, f.get());
    // TEST_ASSERT_EQUAL(7, f.get());
    TEST_ASSERT_EQUAL(8, f.circular_get());
    TEST_ASSERT_EQUAL(9, f.circular_get());
    TEST_ASSERT_EQUAL(10, f.circular_get());
    TEST_ASSERT_EQUAL(11, f.circular_get());
    TEST_ASSERT_EQUAL(12, f.circular_get());

    // 4 Cleanup
}

void tests(void){
    // 1 Setup
    Fifo f;
    f.circular_put(1);
    f.circular_put(2);
    f.circular_put(3);
    f.circular_put(4);
    f.circular_put(5);

    // 2-3 Execute and validate
    TEST_ASSERT_EQUAL(1,f.circular_get());
    TEST_ASSERT_EQUAL(2,f.circular_get());
    TEST_ASSERT_EQUAL(3,f.circular_get());
    TEST_ASSERT_EQUAL(4,f.circular_get());
    TEST_ASSERT_EQUAL(5,f.circular_get());
    TEST_ASSERT_EQUAL(1,f.circular_get());
    TEST_ASSERT_EQUAL(2,f.circular_get());
}

int main()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    _delay_ms(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!

    RUN_TEST(test_reset);
    RUN_TEST(test_normal_flow);
    RUN_TEST(test_underflow);
    RUN_TEST(test_overflow);
    RUN_TEST(test_overwrite);

    RUN_TEST(circular_test_reset);
    RUN_TEST(circular_test_normal_flow);
    RUN_TEST(circular_test_underflow);
    RUN_TEST(circular_test_overflow);
    RUN_TEST(circular_test_overwrite);

    RUN_TEST(test_circ_reset);
    RUN_TEST(test_circ_normal_flow);
    RUN_TEST(test_circ_underflow);
    RUN_TEST(test_circ_overflow);
    RUN_TEST(test_circ_overwrite);
    RUN_TEST(tests);
    // Add more unit tests here

    UNITY_END(); // stop unit testing
}