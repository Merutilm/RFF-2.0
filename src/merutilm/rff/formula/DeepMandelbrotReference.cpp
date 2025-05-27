//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMandelbrotReference.h"

#include "../calc/double_exp_math.h"
#include "../calc/rff_math.h"
#include "../mrthy/ArrayCompressor.h"
#include "../ui/RFF.h"

DeepMandelbrotReference::DeepMandelbrotReference(fp_complex &&center, std::vector<double_exp> &&refReal,
                                                 std::vector<double_exp> &&refImag,
                                                 std::vector<ArrayCompressionTool> &&compressor,
                                                 std::vector<uint64_t> &&period, fp_complex &&lastReference,
                                                 fp_complex &&fpgBn) : MandelbrotReference(std::move(center),
                                                                           std::move(compressor), std::move(period),
                                                                           std::move(lastReference), std::move(fpgBn)),
                                                                       refReal(std::move(refReal)),
                                                                       refImag(std::move(refImag)) {
}


/**
 * Generates Reference of Mandelbrot set.
 * @param state the processor
 * @param calc calculation settings
 * @param exp10 the exponent of 10 for arbitrary-precision operation
 * @param initialPeriod the initial period. default value is 0. i.e. maximum iterations of arbitrary-precision operation
 * @param dcMax the length of center-to-vertex of screen.
 * @param strictFPG use arbitrary-precision operation for fpg_bn calculation
 * @param actionPerRefCalcIteration the action of every iteration
 * @return the result of generation. but returns @code PROCESS_TERMINATED_REFERENCE@endcode if the process is terminated
 */
std::unique_ptr<DeepMandelbrotReference> DeepMandelbrotReference::createReference(
    const ParallelRenderState &state, const CalculationSettings &calc, int exp10, uint64_t initialPeriod,
    double_exp dcMax,
    const bool strictFPG, std::function<void(uint64_t)> &&actionPerRefCalcIteration) {
    if (state.interruptRequested()) {
        return RFF::NullPointer::PROCESS_TERMINATED_REFERENCE;
    }

    auto rr = std::vector<double_exp>();
    auto ri = std::vector<double_exp>();
    rr.push_back(double_exp::DEX_ZERO);
    ri.push_back(double_exp::DEX_ZERO);

    fp_complex center = calc.center;
    auto c = center.edit(exp10);
    auto z = fp_complex_calculator(0, 0, exp10);
    auto fpgBn = fp_complex_calculator(0, 0, exp10);
    auto one = fp_complex_calculator(1.0, 0.0, exp10);
    double bailoutSqr = calc.bailout * calc.bailout;

    double_exp fpgBnr = double_exp::DEX_ONE;
    double_exp fpgBni = double_exp::DEX_ZERO;

    uint64_t iteration = 0;
    double_exp zr = double_exp::DEX_ZERO;
    double_exp zi = double_exp::DEX_ZERO;
    uint64_t period = 1;
    auto periodArray = std::vector<uint64_t>();

    double_exp minZRadius = double_exp::DEX_ONE;
    uint64_t reuseIndex = 0;

    auto tools = std::vector<ArrayCompressionTool>();
    uint64_t compressed = 0;
    uint64_t maxIteration = calc.maxIteration;
    auto [compressCriteria, compressionThresholdPower, withoutNormalize] = calc.referenceCompressionSettings;
    auto func = std::move(actionPerRefCalcIteration);

    double compressionThreshold = compressionThresholdPower <= 0 ? 0 : pow(10, -compressionThresholdPower);
    bool canReuse = withoutNormalize;
    auto temps = std::array<double_exp, 8>();

    while ((iteration == 0 || dex_trigonometric::hypot2(zr, zi) < bailoutSqr) && iteration < maxIteration) {
        if (iteration % RFF::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
            return RFF::NullPointer::PROCESS_TERMINATED_REFERENCE;
        }

        // use Fast-Period-Guessing, and create MPA Table
        if (iteration > 0) {
            dex_trigonometric::hypot2(&temps[0], zr, zi);
            double_exp::dex_div(&temps[1], temps[0], dcMax);
            double_exp::dex_mul(&temps[2], fpgBnr, zr);
            double_exp::dex_mul_2exp(&temps[2], temps[2], 1);
            double_exp::dex_mul(&temps[3], fpgBni, zi);
            double_exp::dex_mul_2exp(&temps[3], temps[3], 1);
            double_exp::dex_sub(&temps[2], temps[2], temps[3]);
            double_exp::dex_add(&temps[2], temps[2], double_exp::DEX_ONE);
            double_exp::dex_mul(&temps[3], fpgBnr, zi);
            double_exp::dex_mul_2exp(&temps[3], temps[3], 1);
            double_exp::dex_mul(&temps[4], fpgBni, zr);
            double_exp::dex_mul_2exp(&temps[4], temps[4], 1);
            double_exp::dex_add(&temps[3], temps[3], temps[4]);
            dex_trigonometric::hypot_approx(&temps[4], temps[2], temps[3]);

            temps[2].try_normalize();
            temps[3].try_normalize();
            temps[4].try_normalize();

            double_exp::dex_sub(&temps[5], minZRadius, temps[0]);
            if (minZRadius.isinf() || temps[5].sgn() == 1) {
                double_exp::dex_cpy(&minZRadius, temps[0]);
                periodArray.push_back(iteration);
            }
            double_exp::dex_sub(&temps[4], temps[4], temps[1]);

            if (temps[4].sgn() == 1 || iteration == maxIteration - 1 || (
                    initialPeriod != 0 && initialPeriod == iteration)) {
                periodArray.push_back(iteration);
                break;
            }

            double_exp::dex_cpy(&fpgBnr, temps[2]);
            double_exp::dex_cpy(&fpgBni, temps[3]);
        }

        if (strictFPG) {
            fpgBn *= z.doubled();
            fpgBn += one;
            z.halved();
        }

        //Let's do arbitrary-precision operation!!
        func(iteration);
        z.square();
        z += c;
        z.getReal().double_exp_value(&zr);
        z.getImag().double_exp_value(&zi);

        uint64_t j = iteration;

        if (!withoutNormalize) {
            for (uint64_t i = periodArray.size(); i > 0; --i) {
                if (compressCriteria >= periodArray[i - 1]) {
                    break;
                }
                j %= periodArray[i - 1];

                if (j == 0) {
                    canReuse = true;
                    break;
                }

                if (j == periodArray[i - 1] - 1) {
                    canReuse = false;
                    break;
                }
            }
        }


        if (compressCriteria > 0 && iteration >= 1) {

            const int64_t refIndex = ArrayCompressor::compress(tools, reuseIndex + 1);
            double_exp::dex_div(&temps[0], zr, rr[refIndex]);
            double_exp::dex_div(&temps[1], zi, ri[refIndex]);
            if (
                std::fabs(static_cast<double>(temps[0]) - 1) <= compressionThreshold &&
                std::fabs(static_cast<double>(temps[1]) - 1) <= compressionThreshold && canReuse
            ) {
                ++reuseIndex;
            } else if (reuseIndex != 0) {
                if (reuseIndex > compressCriteria) {
                    // reference compression criteria

                    const auto compressor = ArrayCompressionTool(
                        1, iteration - reuseIndex + 1, iteration);
                    compressed += compressor.range(); //get the increment of iteration
                    tools.push_back(compressor);
                }
                //If it is enough to large, set all reference in the range to 0 and save the index

                reuseIndex = 0;
                canReuse = withoutNormalize;
            }
        }

        period = ++iteration;

        if (compressCriteria == 0 || reuseIndex <= compressCriteria) {
            if (const uint64_t index = iteration - compressed;
                index == rr.size()) {
                rr.push_back(zr);
                ri.push_back(zi);
            } else {
                rr[index] = zr;
                ri[index] = zi;
            }
        }
    }

    if (!strictFPG) {
        fpgBn = fp_complex_calculator(fpgBnr, fpgBni, exp10);
    }

    rr.resize(period - compressed + 1);
    ri.resize(period - compressed + 1);
    rr.shrink_to_fit();
    ri.shrink_to_fit();
    periodArray = periodArray.empty() ? std::vector(1, period) : periodArray;

    return std::make_unique<DeepMandelbrotReference>(std::move(center), std::move(rr), std::move(ri), std::move(tools),
                                                     std::move(periodArray), fp_complex(z), fp_complex(fpgBn));
}


double_exp DeepMandelbrotReference::real(const uint64_t refIteration) const {
    return refReal[ArrayCompressor::compress(compressor, refIteration)];
}

double_exp DeepMandelbrotReference::imag(const uint64_t refIteration) const {
    return refImag[ArrayCompressor::compress(compressor, refIteration)];
}


size_t DeepMandelbrotReference::length() const {
    return refReal.size();
}


uint64_t DeepMandelbrotReference::longestPeriod() const {
    return period.back();
}
