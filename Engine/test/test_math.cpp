//=============================================================================
//
// Adventure Game Studio (AGS)
//
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
//
//=============================================================================

#include "core/platform.h"
#ifdef AGS_RUN_TESTS

#include "gtest/gtest.h"
#include "util/scaling.h"

using namespace AGS::Common;
using namespace AGS::Engine;

void Test_Scaling(int src, int dst)
{
    int x;
    AxisScaling sc;
    sc.Init(src, dst);
    x = sc.ScalePt(0);
    ASSERT_TRUE(x == 0);
    x = sc.ScalePt(src);
    ASSERT_TRUE(x == dst);
    x = sc.UnScalePt(dst);
    ASSERT_TRUE(x == src);
}

TEST(Math,Scaling)
{
    {
        Test_Scaling(100, 100);

        Test_Scaling(100, 1000);
        Test_Scaling(320, 1280);
        Test_Scaling(200, 400);

        Test_Scaling(1000, 100);
        Test_Scaling(1280, 320);
        Test_Scaling(400,  200);

        Test_Scaling(300, 900);
        Test_Scaling(100, 700);
        Test_Scaling(200, 2200);

        Test_Scaling(900, 300);
        Test_Scaling(700, 100);
        Test_Scaling(2200, 200);

        for (int i = 250; i < 2000; i += 25)
        {
            Test_Scaling(200, i);
            Test_Scaling(i, 200);
        }
    }
}

#endif // AGS_RUN_TESTS
