//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <vector>

#include "ArrayCompressionTool.h"
#include "../formula/LightMandelbrotReference.h"

struct LightPA {
    const double anr;
    const double ani;
    const double bnr;
    const double bni;
    const uint64_t skip;
    const double radius;

    LightPA(double anr, double ani, double bnr, double bni, uint64_t skip, double radius);

    class Generator {
        double anr;
        double ani;
        double bnr;
        double bni;
        uint64_t skip;
        double radius;
        uint64_t start;
        const std::vector<ArrayCompressionTool> &compressors;
        const std::vector<double> &refReal;
        const std::vector<double> &refImag;
        double epsilon;
        double dcMax;

public:
        explicit Generator(const LightMandelbrotReference &reference, double epsilon, double dcMax, uint64_t start);

        static std::unique_ptr<Generator> create(const LightMandelbrotReference &reference, const double epsilon, const double dcMax,
                              const uint64_t start) {
            return std::make_unique<Generator>(reference, epsilon, dcMax, start);
        }

        uint64_t getStart() const {
            return start;
        }

        uint64_t getSkip() const {
            return skip;
        }


        void merge(const LightPA &pa);

        void step();

        LightPA build() const {
            return LightPA(anr, ani, bnr, bni, skip, radius);
        };
    };

    bool isValid(double dzRad) const;
};
