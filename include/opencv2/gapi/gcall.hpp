// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2018 Intel Corporation


#ifndef OPENCV_GAPI_GCALL_HPP
#define OPENCV_GAPI_GCALL_HPP

#include <opencv2/gapi/garg.hpp>      // GArg
#include <opencv2/gapi/gmat.hpp>      // GMat
#include <opencv2/gapi/gscalar.hpp>   // GScalar
#include <opencv2/gapi/gframe.hpp>    // GFrame
#include <opencv2/gapi/garray.hpp>    // GArray<T>
#include <opencv2/gapi/gopaque.hpp>   // GOpaque<T>

namespace cv {

struct GKernel;

// The whole idea of this class is to represent an operation
// which is applied to arguments. This is part of public API,
// since it is what users should use to define kernel interfaces.

class GAPI_EXPORTS GCall final
{
public:
    class Priv;

    explicit GCall(const GKernel &k);
    ~GCall();

    template<typename... Ts>
    GCall& pass(Ts&&... args)
    {
        setArgs({GArg(std::move(args))...});
        return *this;
    }

    // A generic yield method - obtain a link to operator's particular GMat output
    GMat    yield      (int output = 0);
    GMatP   yieldP     (int output = 0);
    GScalar yieldScalar(int output = 0);
    GFrame  yieldFrame (int output = 0);

    template<class T> GArray<T> yieldArray(int output = 0)
    {
        return GArray<T>(yieldArray(output));
    }

    template<class T> GOpaque<T> yieldOpaque(int output = 0)
    {
        return GOpaque<T>(yieldOpaque(output));
    }

    // Internal use only
    Priv& priv();
    const Priv& priv() const;

    // GKernel and params can be modified, it's needed for infer<Generic>,
    // because information about output shapes doesn't exist in compile time
    GKernel& kernel();
    util::any& params();

    void setArgs(std::vector<GArg> &&args);

protected:
    std::shared_ptr<Priv> m_priv;

    // Public versions return a typed array or opaque, those are implementation details
    detail::GArrayU yieldArray(int output = 0);
    detail::GOpaqueU yieldOpaque(int output = 0);
};

} // namespace cv

#endif // OPENCV_GAPI_GCALL_HPP
