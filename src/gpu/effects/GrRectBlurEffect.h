/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * This file was autogenerated from GrRectBlurEffect.fp; do not modify.
 */
#ifndef GrRectBlurEffect_DEFINED
#define GrRectBlurEffect_DEFINED
#include "SkTypes.h"
#if SK_SUPPORT_GPU

#include "GrProxyProvider.h"
#include "../effects/SkBlurMask.h"
#include "GrFragmentProcessor.h"
#include "GrCoordTransform.h"
class GrRectBlurEffect : public GrFragmentProcessor {
public:
    static sk_sp<GrTextureProxy> CreateBlurProfileTexture(GrProxyProvider* proxyProvider,
                                                          float sigma) {
        unsigned int profileSize = SkScalarCeilToInt(6 * sigma);

        static const GrUniqueKey::Domain kDomain = GrUniqueKey::GenerateDomain();
        GrUniqueKey key;
        GrUniqueKey::Builder builder(&key, kDomain, 1);
        builder[0] = profileSize;
        builder.finish();

        sk_sp<GrTextureProxy> blurProfile(
                proxyProvider->findOrCreateProxyByUniqueKey(key, kTopLeft_GrSurfaceOrigin));
        if (!blurProfile) {
            GrSurfaceDesc texDesc;
            texDesc.fOrigin = kTopLeft_GrSurfaceOrigin;
            texDesc.fWidth = profileSize;
            texDesc.fHeight = 1;
            texDesc.fConfig = kAlpha_8_GrPixelConfig;

            std::unique_ptr<uint8_t[]> profile(SkBlurMask::ComputeBlurProfile(sigma));

            blurProfile =
                    proxyProvider->createTextureProxy(texDesc, SkBudgeted::kYes, profile.get(), 0);
            if (!blurProfile) {
                return nullptr;
            }

            SkASSERT(blurProfile->origin() == kTopLeft_GrSurfaceOrigin);
            proxyProvider->assignUniqueKeyToProxy(key, blurProfile.get());
        }

        return blurProfile;
    }
    SkRect rect() const { return fRect; }
    float sigma() const { return fSigma; }

    static std::unique_ptr<GrFragmentProcessor> Make(GrProxyProvider* proxyProvider,
                                                     const SkRect& rect, float sigma) {
        int doubleProfileSize = SkScalarCeilToInt(12 * sigma);

        if (doubleProfileSize >= rect.width() || doubleProfileSize >= rect.height()) {
            // if the blur sigma is too large so the gaussian overlaps the whole
            // rect in either direction, fall back to CPU path for now.
            return nullptr;
        }

        sk_sp<GrTextureProxy> blurProfile(CreateBlurProfileTexture(proxyProvider, sigma));
        if (!blurProfile) {
            return nullptr;
        }

        return std::unique_ptr<GrFragmentProcessor>(new GrRectBlurEffect(
                rect, sigma, std::move(blurProfile),
                GrSamplerState(GrSamplerState::WrapMode::kClamp, GrSamplerState::Filter::kBilerp)));
    }
    GrRectBlurEffect(const GrRectBlurEffect& src);
    std::unique_ptr<GrFragmentProcessor> clone() const override;
    const char* name() const override { return "RectBlurEffect"; }

private:
    GrRectBlurEffect(SkRect rect, float sigma, sk_sp<GrTextureProxy> blurProfile,
                     GrSamplerState samplerParams)
            : INHERITED(kGrRectBlurEffect_ClassID,
                        (OptimizationFlags)kCompatibleWithCoverageAsAlpha_OptimizationFlag)
            , fRect(rect)
            , fSigma(sigma)
            , fBlurProfile(std::move(blurProfile), samplerParams) {
        this->addTextureSampler(&fBlurProfile);
    }
    GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
    void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
    bool onIsEqual(const GrFragmentProcessor&) const override;
    GR_DECLARE_FRAGMENT_PROCESSOR_TEST
    SkRect fRect;
    float fSigma;
    TextureSampler fBlurProfile;
    typedef GrFragmentProcessor INHERITED;
};
#endif
#endif
