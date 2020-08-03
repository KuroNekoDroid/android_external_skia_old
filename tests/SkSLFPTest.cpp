/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/sksl/SkSLCompiler.h"
#include "src/sksl/SkSLStringStream.h"

#include "tests/Test.h"

static void test(skiatest::Reporter* r, const GrShaderCaps& caps, const char* src,
                 std::vector<const char*> expectedH, std::vector<const char*> expectedCPP) {
    SkSL::Program::Settings settings;
    settings.fCaps = &caps;
    settings.fRemoveDeadFunctions = false;
    SkSL::Compiler compiler;
    SkSL::StringStream output;
    std::unique_ptr<SkSL::Program> program = compiler.convertProgram(
                                                             SkSL::Program::kFragmentProcessor_Kind,
                                                             SkSL::String(src),
                                                             settings);
    if (!program) {
        SkDebugf("Unexpected error compiling %s\n%s", src, compiler.errorText().c_str());
        return;
    }
    REPORTER_ASSERT(r, program);
    bool success = compiler.toH(*program, "Test", output);
    if (!success) {
        SkDebugf("Unexpected error compiling %s\n%s", src, compiler.errorText().c_str());
    }
    REPORTER_ASSERT(r, success);
    if (success) {
        for (const char* expected : expectedH) {
            bool found = strstr(output.str().c_str(), expected);
            if (!found) {
                SkDebugf("HEADER MISMATCH:\nsource:\n%s\n\n"
                         "header expected:\n'%s'\n\n"
                         "header received:\n'%s'",
                         src, expected, output.str().c_str());
            }
            REPORTER_ASSERT(r, found);
        }
    }
    output.reset();
    success = compiler.toCPP(*program, "Test", output);
    if (!success) {
        SkDebugf("Unexpected error compiling %s\n%s", src, compiler.errorText().c_str());
    }
    REPORTER_ASSERT(r, success);
    if (success) {
        for (const char* expected : expectedCPP) {
            bool found = strstr(output.str().c_str(), expected);
            if (!found) {
                SkDebugf("CPP MISMATCH:\nsource:\n%s\n\n"
                         "cpp expected:\n'%s'\n\n"
                         "cpp received:\n'%s'",
                         src, expected, output.str().c_str());
            }
            REPORTER_ASSERT(r, found);
        }
    }
}

static void test_failure(skiatest::Reporter* r, const char* src, const char* error) {
    SkSL::Compiler compiler;
    SkSL::Program::Settings settings;
    sk_sp<GrShaderCaps> caps = SkSL::ShaderCapsFactory::Default();
    settings.fCaps = caps.get();
    std::unique_ptr<SkSL::Program> program = compiler.convertProgram(
                                                             SkSL::Program::kFragmentProcessor_Kind,
                                                             SkSL::String(src),
                                                             settings);
    if (!compiler.errorCount()) {
        compiler.optimize(*program);
    }
    SkSL::String skError(error);
    if (compiler.errorText() != skError) {
        SkDebugf("SKSL ERROR:\n    source: %s\n    expected: %s    received: %s",
                 src, error, compiler.errorText().c_str());
    }
    REPORTER_ASSERT(r, compiler.errorText() == skError);
}

DEF_TEST(SkSLFPHelloWorld, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             /* HELLO WORLD */
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{
R"__Header__(/* HELLO WORLD */

/**************************************************************************************************
 *** This file was autogenerated from GrTest.fp; do not modify.
 **************************************************************************************************/
#ifndef GrTest_DEFINED
#define GrTest_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"


#include "src/gpu/GrFragmentProcessor.h"

class GrTest : public GrFragmentProcessor {
public:
    static std::unique_ptr<GrFragmentProcessor> Make() {
        return std::unique_ptr<GrFragmentProcessor>(new GrTest());
    }
    GrTest(const GrTest& src);
    std::unique_ptr<GrFragmentProcessor> clone() const override;
    const char* name() const override { return "Test"; }
private:
    GrTest()
    : INHERITED(kGrTest_ClassID, kNone_OptimizationFlags) {
    }
    GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
    void onGetGLSLProcessorKey(const GrShaderCaps&,GrProcessorKeyBuilder*) const override;
    bool onIsEqual(const GrFragmentProcessor&) const override;
    GR_DECLARE_FRAGMENT_PROCESSOR_TEST
    typedef GrFragmentProcessor INHERITED;
};
#endif
)__Header__"
         },
         /*expectedCPP=*/{
R"__Cpp__(/* HELLO WORLD */

/**************************************************************************************************
 *** This file was autogenerated from GrTest.fp; do not modify.
 **************************************************************************************************/
#include "GrTest.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLTest : public GrGLSLFragmentProcessor {
public:
    GrGLSLTest() {}
    void emitCode(EmitArgs& args) override {
        GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
        const GrTest& _outer = args.fFp.cast<GrTest>();
        (void) _outer;
        fragBuilder->codeAppendf(
R"SkSL(%s = half4(1.0);
)SkSL"
, args.fOutputColor);
    }
private:
    void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
    }
};
GrGLSLFragmentProcessor* GrTest::onCreateGLSLInstance() const {
    return new GrGLSLTest();
}
void GrTest::onGetGLSLProcessorKey(const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
}
bool GrTest::onIsEqual(const GrFragmentProcessor& other) const {
    const GrTest& that = other.cast<GrTest>();
    (void) that;
    return true;
}
GrTest::GrTest(const GrTest& src)
: INHERITED(kGrTest_ClassID, src.optimizationFlags()) {
        this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrTest::clone() const {
    return std::make_unique<GrTest>(*this);
}
)__Cpp__"
         });
}

DEF_TEST(SkSLFPInputHalf2, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in uniform half2 point;
             void main() {
                 sk_OutColor = half4(point, point);
             }
         )__SkSL__",
         /*expectedH=*/{
             "static std::unique_ptr<GrFragmentProcessor> Make(SkPoint point) {",
             "return std::unique_ptr<GrFragmentProcessor>(new GrTest(point));",
             "GrTest(SkPoint point)",
             ", point(point)"
         },
         /*expectedCPP=*/{
R"__Cpp__(fragBuilder->codeAppendf(
R"SkSL(%s = half4(%s, %s);
)SkSL"
, args.fOutputColor, args.fUniformHandler->getUniformCStr(pointVar), args.fUniformHandler->getUniformCStr(pointVar));
)__Cpp__",
             "if (point != that.point) return false;"
         });
}

DEF_TEST(SkSLFPInputHalf1, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             layout(key) in half value;
             void main() {
                 sk_OutColor = half4(value);
             }
         )__SkSL__",
         /*expectedH=*/{
             "static std::unique_ptr<GrFragmentProcessor> Make(float value) {",
             "return std::unique_ptr<GrFragmentProcessor>(new GrTest(value));",
             "GrTest(float value)",
             ", value(value)"
         },
         /*expectedCPP=*/{
R"__Cpp__(void GrTest::onGetGLSLProcessorKey(const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
    b->add32(sk_bit_cast<uint32_t>(value));
})__Cpp__",
R"__Cpp__(fragBuilder->codeAppendf(
R"SkSL(%s = half4(%f);
)SkSL"
, args.fOutputColor, _outer.value);
)__Cpp__",
             "if (value != that.value) return false;"
         });
}

DEF_TEST(SkSLFPUniform, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             uniform half4 color;
             void main() {
                 sk_OutColor = color;
             }
         )__SkSL__",
         /*expectedH=*/{
             "static std::unique_ptr<GrFragmentProcessor> Make()"
         },
         /*expectedCPP=*/{
             "colorVar = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, "
                                                        "kHalf4_GrSLType, \"color\");",
         });
}

// SkSLFPInUniform tests the simplest plumbing case, default type, no tracking
// with a setUniform template that supports inlining the value call with no
// local variable.
DEF_TEST(SkSLFPInUniform, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in uniform half4 color;
             void main() {
                 sk_OutColor = color;
             }
         )__SkSL__",
         /*expectedH=*/{
             "static std::unique_ptr<GrFragmentProcessor> Make(SkRect color) {",
         },
         /*expectedCPP=*/{
            "colorVar = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, "
                                                        "kHalf4_GrSLType, \"color\");",
            "pdman.set4fv(colorVar, 1, reinterpret_cast<const float*>(&(_outer.color)));"
         });
}

// As above, but tests in uniform's ability to override the default ctype.
DEF_TEST(SkSLFPInUniformCType, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             layout(ctype=SkPMColor4f) in uniform half4 color;
             void main() {
                 sk_OutColor = color;
             }
         )__SkSL__",
         /*expectedH=*/{
             "static std::unique_ptr<GrFragmentProcessor> Make(SkPMColor4f color) {",
         },
         /*expectedCPP=*/{
            "colorVar = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, "
                                                        "kHalf4_GrSLType, \"color\");",
            "pdman.set4fv(colorVar, 1, (_outer.color).vec());"
         });
}

// Add state tracking to the default typed SkRect <-> half4 uniform. But since
// it now has to track state, the value inlining previously done for the
// setUniform call is removed in favor of a local variable.
DEF_TEST(SkSLFPTrackedInUniform, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             layout(tracked) in uniform half4 color;
             void main() {
                 sk_OutColor = color;
             }
         )__SkSL__",
         /*expectedH=*/{
             "static std::unique_ptr<GrFragmentProcessor> Make(SkRect color) {",
         },
         /*expectedCPP=*/{
            "SkRect colorPrev = SkRect::MakeEmpty();",
            "colorVar = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, "
                                                        "kHalf4_GrSLType, \"color\");",
            "const SkRect& colorValue = _outer.color;",
            "if (colorPrev.isEmpty() || colorPrev != colorValue) {",
            "colorPrev = colorValue;",
            "pdman.set4fv(colorVar, 1, reinterpret_cast<const float*>(&colorValue));"
         });
}

// Test the case where the template does not support variable inlining in
// setUniform (i.e. it references the value multiple times).
DEF_TEST(SkSLFPNonInlinedInUniform, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in uniform half2 point;
             void main() {
                 sk_OutColor = half4(point, point);
             }
         )__SkSL__",
         /*expectedH=*/{
             "static std::unique_ptr<GrFragmentProcessor> Make(SkPoint point) {",
         },
         /*expectedCPP=*/{
            "pointVar = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, "
                                                        "kHalf2_GrSLType, \"point\");",
            "const SkPoint& pointValue = _outer.point;",
            "pdman.set2f(pointVar, pointValue.fX, pointValue.fY);"
         });
}

// Test handling conditional uniforms (that use when= in layout), combined with
// state tracking and custom ctypes to really put the code generation through its paces.
DEF_TEST(SkSLFPConditionalInUniform, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             layout(key) in bool test;
             layout(ctype=SkPMColor4f, tracked, when=test) in uniform half4 color;
             void main() {
               if (test) {
                 sk_OutColor = color;
               } else {
                 sk_OutColor = half4(1);
               }
             }
         )__SkSL__",
         /*expectedH=*/{
             "static std::unique_ptr<GrFragmentProcessor> Make(bool test, SkPMColor4f color) {",
         },
         /*expectedCPP=*/{
            "SkPMColor4f colorPrev = {SK_FloatNaN, SK_FloatNaN, SK_FloatNaN, SK_FloatNaN}",
            "auto test = _outer.test;",
            "if (test) {",
            "colorVar = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, "
                                                        "kHalf4_GrSLType, \"color\");",
            "if (colorVar.isValid()) {",
            "const SkPMColor4f& colorValue = _outer.color;",
            "if (colorPrev != colorValue) {",
            "colorPrev = colorValue;",
            "pdman.set4fv(colorVar, 1, colorValue.vec());"
         });
}

DEF_TEST(SkSLFPSections, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @header { header section }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{
             "header section"
         },
         /*expectedCPP=*/{});
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @class { class section }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{
             "class GrTest : public GrFragmentProcessor {\n"
             "public:\n"
             " class section"
         },
         /*expectedCPP=*/{});
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @cpp { cpp section }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{},
         /*expectedCPP=*/{
            "cpp section"
         });
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @constructorParams { int x, float y, std::vector<float> z }
             in float w;
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{
             "Make(float w,  int x, float y, std::vector<float> z )",
             "return std::unique_ptr<GrFragmentProcessor>(new GrTest(w, x, y, z));",
             "GrTest(float w,  int x, float y, std::vector<float> z )",
             ", w(w) {"
         },
         /*expectedCPP=*/{});
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @constructor { constructor section }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{
             "private:\n constructor section"
         },
         /*expectedCPP=*/{});
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @initializers { initializers section }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{
             ": INHERITED(kGrTest_ClassID, kNone_OptimizationFlags)\n    ,  initializers section"
         },
         /*expectedCPP=*/{});
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             half x = 10;
             @emitCode { fragBuilder->codeAppendf("half y = %d\n", x * 2); }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{},
         /*expectedCPP=*/{
            "x = 10.0;\n"
            " fragBuilder->codeAppendf(\"half y = %d\\n\", x * 2);"
         });
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @fields { fields section }
             @clone { }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{
            "const char* name() const override { return \"Test\"; }\n"
            " fields section private:"
         },
         /*expectedCPP=*/{});
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @make { make section }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{
            "public:\n"
            " make section"
         },
         /*expectedCPP=*/{});
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             uniform half calculated;
             layout(key) in half provided;
             @setData(varName) { varName.set1f(calculated, provided * 2); }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{},
         /*expectedCPP=*/{
             "void onSetData(const GrGLSLProgramDataManager& varName, "
                            "const GrFragmentProcessor& _proc) override {\n",
             "UniformHandle& calculated = calculatedVar;",
             "auto provided = _outer.provided;",
             "varName.set1f(calculated, provided * 2);"
         });
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             @test(testDataName) { testDataName section }
             void main() {
                 sk_OutColor = half4(1);
             }
         )__SkSL__",
         /*expectedH=*/{},
         /*expectedCPP=*/{
             "#if GR_TEST_UTILS\n"
             "std::unique_ptr<GrFragmentProcessor> GrTest::TestCreate(GrProcessorTestData* testDataName) {\n"
             " testDataName section }\n"
             "#endif"
         });
}

DEF_TEST(SkSLFPMainCoords, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             void main(float2 coord) {
                 sk_OutColor = half4(coord, coord);
             }
         )__SkSL__",
         /*expectedH=*/{
             "this->setUsesSampleCoordsDirectly();"
         },
         /*expectedCPP=*/{
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(%s = half4(%s, %s);\n"
            ")SkSL\"\n"
            ", args.fOutputColor, args.fSampleCoord, args.fSampleCoord);"
         });
}

DEF_TEST(SkSLFPLayoutWhen, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
            layout(when=someExpression(someOtherExpression())) uniform half sometimes;
            void main() {
            }
         )__SkSL__",
         /*expectedH=*/{},
         /*expectedCPP=*/{
            "if (someExpression(someOtherExpression())) {\n"
            "            sometimesVar = args.fUniformHandler->addUniform"
         });
}

DEF_TEST(SkSLFPChildProcessors, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor child1;
             in fragmentProcessor child2;
             void main() {
                 sk_OutColor = sample(child1) * sample(child2);
             }
         )__SkSL__",
         /*expectedH=*/{
            "this->registerChild(std::move(child1), SkSL::SampleUsage::PassThrough());",
            "this->registerChild(std::move(child2), SkSL::SampleUsage::PassThrough());"
         },
         /*expectedCPP=*/{
            "SkString _sample149 = this->invokeChild(0, args);\n",
            "SkString _sample166 = this->invokeChild(1, args);\n",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(%s = %s * %s;\n"
            ")SkSL\"\n"
            ", args.fOutputColor, _sample149.c_str(), _sample166.c_str());",
            "this->cloneAndRegisterAllChildProcessors(src);",
         });
}

DEF_TEST(SkSLFPChildProcessorsWithInput, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor child1;
             in fragmentProcessor child2;
             void main() {
                 half4 childIn = sk_InColor;
                 half4 childOut1 = sample(child1, childIn);
                 half4 childOut2 = sample(child2, childOut1);
                 sk_OutColor = childOut2;
             }
         )__SkSL__",
         /*expectedH=*/{
            "this->registerChild(std::move(child1), SkSL::SampleUsage::PassThrough());",
            "this->registerChild(std::move(child2), SkSL::SampleUsage::PassThrough());"
         },
         /*expectedCPP=*/{
            "SkString _input198(\"childIn\");",
            "SkString _sample198 = this->invokeChild(0, _input198.c_str(), args);",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(\n"
            "half4 childOut1 = %s;)SkSL\"\n"
            ", _sample198.c_str());",
            "SkString _input258(\"childOut1\");",
            "SkString _sample258 = this->invokeChild(1, _input258.c_str(), args);",
            "this->cloneAndRegisterAllChildProcessors(src);",
         });
}

DEF_TEST(SkSLFPChildProcessorWithInputExpression, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor child;
             void main() {
                 sk_OutColor = sample(child, sk_InColor * half4(0.5));
             }
         )__SkSL__",
         /*expectedH=*/{
            "this->registerChild(std::move(child), SkSL::SampleUsage::PassThrough());",
         },
         /*expectedCPP=*/{
            "SkString _input106 = SkStringPrintf(\"%s * half4(0.5)\", args.fInputColor);",
            "SkString _sample106 = this->invokeChild(0, _input106.c_str(), args);",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(%s = %s;\n"
            ")SkSL\"\n"
            ", args.fOutputColor, _sample106.c_str());",
            "this->cloneAndRegisterAllChildProcessors(src);",
         });
}

DEF_TEST(SkSLFPNestedChildProcessors, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor child1;
             in fragmentProcessor child2;
             void main() {
                 sk_OutColor = sample(child2, sk_InColor * sample(child1, sk_InColor * half4(0.5)));
             }
         )__SkSL__",
         /*expectedH=*/{
            "this->registerChild(std::move(child1), SkSL::SampleUsage::PassThrough());",
            "this->registerChild(std::move(child2), SkSL::SampleUsage::PassThrough());"
         },
         /*expectedCPP=*/{
            "SkString _input177 = SkStringPrintf(\"%s * half4(0.5)\", args.fInputColor);",
            "SkString _sample177 = this->invokeChild(0, _input177.c_str(), args);",
            "SkString _input149 = SkStringPrintf(\"%s * %s\", args.fInputColor, _sample177.c_str());",
            "SkString _sample149 = this->invokeChild(1, _input149.c_str(), args);",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(%s = %s;\n"
            ")SkSL\"\n"
            ", args.fOutputColor, _sample149.c_str());",
            "this->cloneAndRegisterAllChildProcessors(src);",
            });
}

DEF_TEST(SkSLFPChildFPAndGlobal, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor child;
             bool hasCap = sk_Caps.externalTextureSupport;
             void main() {
                 if (hasCap) {
                     sk_OutColor = sample(child, sk_InColor);
                 } else {
                     sk_OutColor = half4(1);
                 }
             }
         )__SkSL__",
         /*expectedH=*/{
            "this->registerChild(std::move(child), SkSL::SampleUsage::PassThrough());"
         },
         /*expectedCPP=*/{
            "hasCap = sk_Caps.externalTextureSupport;",

            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(bool hasCap = %s;\n"
            "if (hasCap) {)SkSL\"\n"
            ", (hasCap ? \"true\" : \"false\"));",
            "SkString _input200(args.fInputColor);",
            "SkString _sample200 = this->invokeChild(0, _input200.c_str(), args);",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(\n"
            "    %s = %s;\n"
            "} else {\n"
            "    %s = half4(1.0);\n"
            "}\n"
            ")SkSL\"\n"
            ", args.fOutputColor, _sample200.c_str(), args.fOutputColor);",
            "this->cloneAndRegisterAllChildProcessors(src);",
         });
}

DEF_TEST(SkSLFPChildProcessorInlineFieldAccess, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor child;
             void main() {
                 if (child.preservesOpaqueInput) {
                     sk_OutColor = sample(child, sk_InColor);
                 } else {
                     sk_OutColor = half4(1);
                 }
             }
         )__SkSL__",
         /*expectedH=*/{
            "this->registerChild(std::move(child), SkSL::SampleUsage::PassThrough());"
         },
         /*expectedCPP=*/{
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(if (%s) {)SkSL\"\n"
            ", (_outer.childProcessor(0)->preservesOpaqueInput() ? \"true\" : \"false\"));",
            "SkString _input161(args.fInputColor);",
            "SkString _sample161 = this->invokeChild(0, _input161.c_str(), args);",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(\n"
            "    %s = %s;\n"
            "} else {\n"
            "    %s = half4(1.0);\n"
            "}\n"
            ")SkSL\"\n"
            ", args.fOutputColor, _sample161.c_str(), args.fOutputColor);",
            "this->cloneAndRegisterAllChildProcessors(src);",
         });
}

DEF_TEST(SkSLFPChildProcessorFieldAccess, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor child;
             bool opaque = child.preservesOpaqueInput;
             void main() {
                 if (opaque) {
                     sk_OutColor = sample(child);
                 } else {
                     sk_OutColor = half4(0.5);
                 }
         }
         )__SkSL__",
         /*expectedH=*/{
            "this->registerChild(std::move(child), SkSL::SampleUsage::PassThrough());"
         },
         /*expectedCPP=*/{
            "opaque = _outer.childProcessor(0)->preservesOpaqueInput();",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(bool opaque = %s;\n"
            "if (opaque) {)SkSL\"\n"
            ", (opaque ? \"true\" : \"false\"));",
            "SkString _sample196 = this->invokeChild(0, args);",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(\n"
            "    %s = %s;\n"
            "} else {\n"
            "    %s = half4(0.5);\n"
            "}\n"
            ")SkSL\"\n"
            ", args.fOutputColor, _sample196.c_str(), args.fOutputColor);",
            "this->cloneAndRegisterAllChildProcessors(src);",
         });
}

DEF_TEST(SkSLFPNullableChildProcessor, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             void main() {
                 if (child != null) {
                     sk_OutColor = sample(child);
                 } else {
                     sk_OutColor = half4(0.5);
                 }
             }
         )__SkSL__",
         /*expectedH=*/{},
         /*expectedCPP=*/{
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(if (%s) {)SkSL\"\n"
            ", _outer.childProcessor(0) ? \"true\" : \"false\");",
            "SkString _sample149 = this->invokeChild(0, args);",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(\n"
            "    %s = %s;\n"
            "} else {\n"
            "    %s = half4(0.5);\n"
            "}\n"
            ")SkSL\"\n"
            ", args.fOutputColor, _sample149.c_str(), args.fOutputColor);",
         });
}

DEF_TEST(SkSLFPBadIn, r) {
    test_failure(r,
         R"__SkSL__(
             in half4 c;
             void main() {
                 sk_OutColor = c;
             }
         )__SkSL__",
         "error: 4: 'in' variable must be either 'uniform' or 'layout(key)', or there must be a "
         "custom @setData function\n1 error\n");
}

DEF_TEST(SkSLFPNoFPLocals, r) {
    test_failure(r,
    R"__SkSL__(
        void main() {
            fragmentProcessor child;
        }
    )__SkSL__",
    "error: 1: variables of type 'fragmentProcessor' must be global\n"
    "1 error\n");
}

DEF_TEST(SkSLFPNoFPParams, r) {
    test_failure(r,
    R"__SkSL__(
        in fragmentProcessor child;
        half4 helper(fragmentProcessor fp) { return sample(fp); }
        void main() {
            sk_OutColor = helper(child);
        }
    )__SkSL__",
    "error: 3: parameters of type 'fragmentProcessor' not allowed\n"
    "error: 5: unknown identifier 'helper'\n"
    "2 errors\n");
}

DEF_TEST(SkSLFPNoFPReturns, r) {
    test_failure(r,
    R"__SkSL__(
        in fragmentProcessor child;
        fragmentProcessor get_child() { return child; }
        void main() {
            sk_OutColor = sample(get_child());
        }
    )__SkSL__",
    "error: 3: functions may not return type 'fragmentProcessor'\n"
    "error: 5: unknown identifier 'get_child'\n"
    "2 errors\n");
}

DEF_TEST(SkSLFPNoFPConstructors, r) {
    test_failure(r,
    R"__SkSL__(
        in fragmentProcessor child;
        void main() {
            sk_OutColor = sample(fragmentProcessor(child));
        }
    )__SkSL__",
    "error: 4: cannot construct 'fragmentProcessor'\n"
    "1 error\n");
}

DEF_TEST(SkSLFPNoFPExpressions, r) {
    test_failure(r,
    R"__SkSL__(
        in fragmentProcessor child1;
        in fragmentProcessor child2;
        void main(float2 coord) {
            sk_OutColor = sample(coord.x > 10 ? child1 : child2);
        }
    )__SkSL__",
    "error: 5: ternary expression of type 'fragmentProcessor' not allowed\n"
    "1 error\n");
}

DEF_TEST(SkSLFPSampleCoords, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor child;
             void main(float2 coord) {
                 sk_OutColor = sample(child) + sample(child, coord / 2);
             }
         )__SkSL__",
         /*expectedH=*/{
             "this->registerChild(std::move(child), SkSL::SampleUsage(SkSL::SampleUsage::Kind::kNone, \"\", false, true, true));",
             "this->setUsesSampleCoordsDirectly();"
         },
         /*expectedCPP=*/{
            "SkString _sample118 = this->invokeChild(0, args);\n",
            "SkString _coords134 = SkStringPrintf(\"%s / 2.0\", args.fSampleCoord);\n",
            "SkString _sample134 = this->invokeChild(0, args, _coords134.c_str());\n",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(%s = %s + %s;\n"
            ")SkSL\"\n"
            ", args.fOutputColor, _sample118.c_str(), _sample134.c_str());"
        });
}

DEF_TEST(SkSLFPFunction, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             half4 flip(half4 c) { return c.abgr; }
             void main() {
                 sk_OutColor = flip(sk_InColor);
             }
         )__SkSL__",
         /*expectedH=*/{},
         /*expectedCPP=*/{
            "SkString flip_name;",
            "const GrShaderVar flip_args[] = { GrShaderVar(\"c\", kHalf4_GrSLType)};",
            "fragBuilder->emitFunction(kHalf4_GrSLType, \"flip\", 1, flip_args,\n"
            "R\"SkSL(return c.wzyx;\n"
            ")SkSL\", &flip_name);",
            "fragBuilder->codeAppendf(\n"
            "R\"SkSL(half4 _inlineResulthalf4fliphalf40;\n"
            "half4 _inlineArghalf4fliphalf41_0 = %s;\n"
            "{\n"
            "    _inlineResulthalf4fliphalf40 = _inlineArghalf4fliphalf41_0.wzyx;\n"
            "}\n"
            "%s = _inlineResulthalf4fliphalf40;\n"
            "\n"
            ")SkSL\"\n"
            ", args.fInputColor, args.fOutputColor);"
         });
}

DEF_TEST(SkSLFPMatrixSampleConstant, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             void main() {
                 sk_OutColor = sample(child, float3x3(2));
             }
         )__SkSL__",
         /*expectedH=*/{
             "this->registerChild(std::move(child), "
                    "SkSL::SampleUsage::UniformMatrix(\"float3x3(2.0)\", true));"
         },
         /*expectedCPP=*/{
             "this->invokeChildWithMatrix(0, args)"
         });
}

DEF_TEST(SkSLFPMatrixSampleUniform, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             uniform float3x3 matrix;
             void main() {
                 sk_OutColor = sample(child, matrix);
             }
         )__SkSL__",
         /*expectedH=*/{
             // Since 'matrix' is just a uniform, the generated code can't determine perspective.
             "this->registerChild(std::move(child), "
                    "SkSL::SampleUsage::UniformMatrix(\"matrix\", true));"
         },
         /*expectedCPP=*/{
             "this->invokeChildWithMatrix(0, args)"
         });
}

DEF_TEST(SkSLFPMatrixSampleInUniform, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             in uniform float3x3 matrix;
             void main() {
                 sk_OutColor = sample(child, matrix);
             }
         )__SkSL__",
         /*expectedH=*/{
             // Since 'matrix' is marked 'in', we can detect perspective at runtime
             "this->registerChild(std::move(child), "
                    "SkSL::SampleUsage::UniformMatrix(\"matrix\", matrix.hasPerspective()));"
         },
         /*expectedCPP=*/{
             "this->invokeChildWithMatrix(0, args)"
         });
}

DEF_TEST(SkSLFPMatrixSampleMultipleInUniforms, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             in uniform float3x3 matrixA;
             in uniform float3x3 matrixB;
             void main() {
                 sk_OutColor = sample(child, matrixA);
                 sk_OutColor += sample(child, matrixB);
             }
         )__SkSL__",
         /*expectedH=*/{
             // FIXME it would be nice if codegen can produce
             // (matrixA.hasPerspective() || matrixB.hasPerspective()) even though it's variable.
             "this->registerChild(std::move(child), "
                    "SkSL::SampleUsage::VariableMatrix(true));"
         },
         /*expectedCPP=*/{
             "SkString _matrix191(args.fUniformHandler->getUniformCStr(matrixAVar));",
             "this->invokeChildWithMatrix(0, args, _matrix191.c_str());",
             "SkString _matrix247(args.fUniformHandler->getUniformCStr(matrixBVar));",
             "this->invokeChildWithMatrix(0, args, _matrix247.c_str());"
         });
}

DEF_TEST(SkSLFPMatrixSampleConstUniformExpression, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             uniform float3x3 matrix;
             void main() {
                 sk_OutColor = sample(child, 0.5 * matrix);
             }
         )__SkSL__",
         /*expectedH=*/{
             // FIXME: "0.5 * matrix" is a uniform expression and could be lifted to the vertex
             // shader, once downstream code is able to properly map 'matrix' within the expression.
             "this->registerChild(std::move(child), "
                    "SkSL::SampleUsage::VariableMatrix(true));"
         },
         /*expectedCPP=*/{
            "SkString _matrix145 = SkStringPrintf(\"0.5 * %s\", "
                    "args.fUniformHandler->getUniformCStr(matrixVar));",
             "this->invokeChildWithMatrix(0, args, _matrix145.c_str());"
         });
}

DEF_TEST(SkSLFPMatrixSampleConstantAndExplicitly, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             void main(float2 coord) {
                 sk_OutColor = sample(child, float3x3(0.5));
                 sk_OutColor = sample(child, coord / 2);
             }
         )__SkSL__",
         /*expectedH=*/{
             "this->registerChild(std::move(child), "
                    "SkSL::SampleUsage(SkSL::SampleUsage::Kind::kUniform, \"float3x3(0.5)\", true, true, false));"
         },
         /*expectedCPP=*/{
             "this->invokeChildWithMatrix(0, args)",
             "SkString _coords180 = SkStringPrintf(\"%s / 2.0\", args.fSampleCoord);",
             "this->invokeChild(0, args, _coords180.c_str())",
         });
}

DEF_TEST(SkSLFPMatrixSampleVariableAndExplicitly, r) {
    test(r,
         *SkSL::ShaderCapsFactory::Default(),
         R"__SkSL__(
             in fragmentProcessor? child;
             void main(float2 coord) {
                 float3x3 matrix = float3x3(sk_InColor.a);
                 sk_OutColor = sample(child, matrix);
                 sk_OutColor = sample(child, coord / 2);
             }
         )__SkSL__",
         /*expectedH=*/{
             "this->registerChild(std::move(child), "
                    "SkSL::SampleUsage(SkSL::SampleUsage::Kind::kVariable, \"\", true, true, false));"
         },
         /*expectedCPP=*/{
             "SkString _matrix178(\"matrix\");",
             "this->invokeChildWithMatrix(0, args, _matrix178.c_str())",
             "SkString _coords232 = SkStringPrintf(\"%s / 2.0\", args.fSampleCoord);",
             "this->invokeChild(0, args, _coords232.c_str()",
         });
}
