// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2018 Intel Corporation


#ifndef OPENCV_GAPI_GTYPED_HPP
#define OPENCV_GAPI_GTYPED_HPP
#if !defined(GAPI_STANDALONE)

#include <vector>

#include <opencv2/gapi/gcomputation.hpp>
#include <opencv2/gapi/gcompiled.hpp>
#include <opencv2/gapi/gproto.hpp>
#include <opencv2/gapi/gcommon.hpp>

namespace cv {

namespace detail
{
    // FIXME: How to prevent coolhackers from extending it by their own types?
    // FIXME: ...Should we care?
    template<typename T> struct ProtoToParam;
    template<> struct ProtoToParam<GMat>    { using type = Mat; };
    template<> struct ProtoToParam<GScalar> { using type = Scalar; };
    template<typename U> struct ProtoToParam<GArray<U> >  { using type = std::vector<U>; };
    template<> struct ProtoToParam<GArray<GMat>>      { using type = std::vector<Mat>; };
    template<typename U> struct ProtoToParam<GOpaque<U> > { using type = U; };
    template<typename T> using ProtoToParamT = typename ProtoToParam<T>::type;

    template<typename T> struct ProtoToMeta;
    template<> struct ProtoToMeta<GMat>     { using type = GMatDesc; };
    template<> struct ProtoToMeta<GScalar>  { using type = GScalarDesc; };
    template<typename U> struct ProtoToMeta<GArray<U> >  { using type = GArrayDesc; };
    template<typename U> struct ProtoToMeta<GOpaque<U> > { using type = GOpaqueDesc; };
    template<typename T> using ProtoToMetaT = typename ProtoToMeta<T>::type;

    //workaround for MSVC 19.0 bug
    template <typename T>
    auto make_default()->decltype(T{}) {return {};}
} // detail

/**
 * @brief This class is a typed wrapper over a regular GComputation.
 *
 * `std::function<>`-like template parameter specifies the graph
 *  signature so methods so the object's constructor, methods like
 *  `apply()` and the derived `GCompiledT::operator()` also become
 *  typed.
 *
 *  There is no need to use cv::gin() or cv::gout() modifiers with
 *  objects of this class.  Instead, all input arguments are followed
 *  by all output arguments in the order from the template argument
 *  signature.
 *
 *  Refer to the following example. Regular (untyped) code is written this way:
 *
 *  @snippet samples/cpp/tutorial_code/gapi/doc_snippets/api_ref_snippets.cpp Untyped_Example
 *
 *  Here:
 *
 *  - cv::GComputation object is created with a lambda constructor
 *    where it is defined as a two-input, one-output graph.
 *
 *  - Its method `apply()` in fact takes arbitrary number of arguments
 *    (as vectors) so user can pass wrong number of inputs/outputs
 *    here. C++ compiler wouldn't notice that since the cv::GComputation
 *    API is polymorphic, and only a run-time error will be generated.
 *
 *  Now the same code written with typed API:
 *
 *  @snippet samples/cpp/tutorial_code/gapi/doc_snippets/api_ref_snippets.cpp Typed_Example
 *
 *  The key difference is:
 *
 *  - Now the constructor lambda *must take* parameters and *must
 *    return* values as defined in the `GComputationT<>` signature.
 *  - Its method `apply()` does not require any extra specifiers to
 *    separate input arguments from the output ones
 *  - A `GCompiledT` (compilation product) takes input/output
 *    arguments with no extra specifiers as well.
 */
template<typename> class GComputationT;

// Single return value implementation
template<typename R, typename... Args> class GComputationT<R(Args...)>
{
public:
    typedef std::function<R(Args...)> Gen;

    class GCompiledT
    {
    private:
        friend class GComputationT<R(Args...)>;

        GCompiled m_comp;

        explicit GCompiledT(const GCompiled &comp) : m_comp(comp) {}

    public:
        GCompiledT() {}

        void operator()(detail::ProtoToParamT<Args>... inArgs,
                        detail::ProtoToParamT<R> &outArg)
        {
            m_comp(cv::gin(inArgs...), cv::gout(outArg));
        }

        explicit operator bool() const
        {
            return static_cast<bool>(m_comp);
        }
    };

private:
    typedef std::pair<R, GProtoInputArgs > Captured;

    Captured capture(const Gen& g, Args... args)
    {
        return Captured(g(args...), cv::GIn(args...));
    }

    Captured m_capture;
    GComputation m_comp;

public:
    GComputationT(const Gen &generator)
        : m_capture(capture(generator, detail::make_default<Args>()...))
        , m_comp(GProtoInputArgs(std::move(m_capture.second)),
                 cv::GOut(m_capture.first))
    {
    }

    void apply(detail::ProtoToParamT<Args>... inArgs,
               detail::ProtoToParamT<R> &outArg,
               GCompileArgs &&args)
    {
        m_comp.apply(cv::gin(inArgs...), cv::gout(outArg), std::move(args));
    }

    void apply(detail::ProtoToParamT<Args>... inArgs,
               detail::ProtoToParamT<R> &outArg)
    {
        apply(inArgs..., outArg, GCompileArgs());
    }


    GCompiledT compile(detail::ProtoToMetaT<Args>... inDescs)
    {
        GMetaArgs inMetas = { GMetaArg(inDescs)... };
        return GCompiledT(m_comp.compile(std::move(inMetas), GCompileArgs()));
    }

    GCompiledT compile(detail::ProtoToMetaT<Args>... inDescs, GCompileArgs &&args)
    {
        GMetaArgs inMetas = { GMetaArg(inDescs)... };
        return GCompiledT(m_comp.compile(std::move(inMetas), std::move(args)));
    }
};

// Multiple (fixed) return value implementation. FIXME: How to avoid copy-paste?
template<typename... R, typename... Args> class GComputationT<std::tuple<R...>(Args...)>
{
public:
    typedef std::function<std::tuple<R...>(Args...)> Gen;

    class GCompiledT
    {
    private:
        friend class GComputationT<std::tuple<R...>(Args...)>;

        GCompiled m_comp;
        explicit GCompiledT(const GCompiled &comp) : m_comp(comp) {}

    public:
        GCompiledT() {}

        void operator()(detail::ProtoToParamT<Args>... inArgs,
                        detail::ProtoToParamT<R>&... outArgs)
        {
            m_comp(cv::gin(inArgs...), cv::gout(outArgs...));
        }

        explicit operator bool() const
        {
            return static_cast<bool>(m_comp);
        }
    };

private:
    typedef std::pair<GProtoArgs, GProtoArgs> Captured;

    template<int... IIs>
    Captured capture(GProtoArgs &&args, const std::tuple<R...> &rr, detail::Seq<IIs...>)
    {
        return Captured(cv::GOut(std::get<IIs>(rr)...).m_args, args);
    }

    Captured capture(const Gen& g, Args... args)
    {
        return capture(cv::GIn(args...).m_args, g(args...), typename detail::MkSeq<sizeof...(R)>::type());
    }

    Captured m_capture;
    GComputation m_comp;

public:
    GComputationT(const Gen &generator)
        : m_capture(capture(generator, detail::make_default<Args>()...))
        , m_comp(GProtoInputArgs(std::move(m_capture.second)),
                 GProtoOutputArgs(std::move(m_capture.first)))
    {
    }

    void apply(detail::ProtoToParamT<Args>... inArgs,
               detail::ProtoToParamT<R>&... outArgs,
               GCompileArgs &&args)
    {
        m_comp.apply(cv::gin(inArgs...), cv::gout(outArgs...), std::move(args));
    }

    void apply(detail::ProtoToParamT<Args>... inArgs,
               detail::ProtoToParamT<R>&... outArgs)
    {
        apply(inArgs..., outArgs..., GCompileArgs());
    }


    GCompiledT compile(detail::ProtoToMetaT<Args>... inDescs)
    {
        GMetaArgs inMetas = { GMetaArg(inDescs)... };
        return GCompiledT(m_comp.compile(std::move(inMetas), GCompileArgs()));
    }

    GCompiledT compile(detail::ProtoToMetaT<Args>... inDescs, GCompileArgs &&args)
    {
        GMetaArgs inMetas = { GMetaArg(inDescs)... };
        return GCompiledT(m_comp.compile(std::move(inMetas), std::move(args)));
    }
};

} // namespace cv
#endif // !defined(GAPI_STANDALONE)
#endif // OPENCV_GAPI_GTYPED_HPP
