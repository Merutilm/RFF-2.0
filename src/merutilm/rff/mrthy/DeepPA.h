//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "ArrayCompressionTool.h"
#include "PA.h"
#include "../calc/double_exp.h"
#include "../formula/DeepMandelbrotReference.h"


struct DeepPA final : public PA{
    const double_exp anr;
    const double_exp ani;
    const double_exp bnr;
    const double_exp bni;
    const uint64_t skip;
    const double_exp radius;

    DeepPA(const double_exp &anr, const double_exp &ani, const double_exp &bnr, const double_exp &bni, uint64_t skip, const double_exp &radius);

    class Generator {
        double_exp anr;
        double_exp ani;
        double_exp bnr;
        double_exp bni;
        uint64_t skip;
        double_exp radius;
        uint64_t start;
        const std::vector<ArrayCompressionTool> &compressors;
        const std::vector<double_exp> &refReal;
        const std::vector<double_exp> &refImag;
        double epsilon;
        double_exp dcMax;

    public:
        explicit Generator(const DeepMandelbrotReference &reference, double epsilon, const double_exp &dcMax, uint64_t start);

        static std::unique_ptr<Generator> create(const DeepMandelbrotReference &reference, double epsilon,
                                                 const double_exp &dcMax,
                                                 uint64_t start);

        uint64_t getStart() const;

        uint64_t getSkip() const;


        void merge(const DeepPA &pa);

        void step();

        DeepPA build() const {
            return DeepPA(anr, ani, bnr, bni, skip, radius);
        }
    };

    bool isValid(double_exp *temp, const double_exp &dzRad) const;

    double_exp getRadius() const;

};

inline std::unique_ptr<DeepPA::Generator> DeepPA::Generator::create(const DeepMandelbrotReference &reference,
                                                                      const double epsilon, const double_exp &dcMax,
                                                                      const uint64_t start) {
    return std::make_unique<Generator>(reference, epsilon, dcMax, start);
}

inline uint64_t DeepPA::Generator::getStart() const {
    return start;
}

inline uint64_t DeepPA::Generator::getSkip() const {
    return skip;
}

inline double_exp DeepPA::getRadius() const {
    return radius;
}


inline bool DeepPA::isValid(double_exp *temp, const double_exp &dzRad) const {
    double_exp::dex_sub(temp, radius, dzRad);
    return temp->sgn() > 0;
}


