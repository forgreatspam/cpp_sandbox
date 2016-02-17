#pragma once
#include <string>
#include <iostream>
#include <cmath>

#include "util/zip.h"
#include "types.h"
#include "estimate.h"


namespace rnd
{
  // Accepts raw estimate parameters and pass them to 'Printer' class as estimate and variance
  template <class Printer>
  struct PrinterWrapper
  {
    PrinterWrapper(Printer printer) : printer_(printer)
    {}

    void operator()(std::string const & name, linear::Equation const & equation,
                    Estimate const & estimate, size_t repeat) const
    {
      auto const solutionEstimate = GetSolution_(estimate.sum, repeat);
      auto const variance = GetVariance_(solutionEstimate, estimate.sumSq, repeat);

      printer_(name, equation, solutionEstimate, variance);
    }

  private:
    Printer printer_;

    static auto GetSolution_(linear::Vector const & sum, size_t repeat)
    {
      auto estimate(sum);
      for (auto & x : estimate)
        x /= repeat;
      return estimate;
    }

    static auto GetVariance_(linear::Vector const & estimate, linear::Vector const & sumSq, size_t repeat)
    {
      linear::Vector variance{};
      for (auto & x : util::zip(variance, estimate, sumSq))
        x[0] = std::sqrt(x[2] / repeat - std::pow(x[1], 2));
      return variance;
    }
  };


  std::ostream & operator << (std::ostream & stream, linear::Vector const & vect)
  {
    for (auto const & x : vect)
      stream << x << " ";
    return stream << std::endl;
  }


  struct DetailsPrinter
  {
    DetailsPrinter(std::ostream & stream) : stream_(stream)
    {}

    void operator()(std::string const & name, linear::Equation const & equation,
                    linear::Vector const & estimate, linear::Vector const & variance) const
    {
      stream_ << name << std::endl;

      stream_ << estimate;
      stream_ << "GetNorm of estimate: " << GetNorm(estimate) << std::endl << std::endl;

      stream_ << variance;
      stream_ << "GetNorm of variance: " << GetNorm(variance) << std::endl << std::endl;

      auto const discrepancy = GetDiscrepancy(equation, estimate);
      stream_ << "GetNorm of discrepancy: " << GetNorm(discrepancy) << std::endl;
    }

  private:
    std::ostream & stream_;
  };


  struct DiscrepancyNormPrinter
  {
    DiscrepancyNormPrinter(std::ostream & stream) : stream_(stream)
    {}

    void operator()(std::string const & name, linear::Equation const & equation,
                    linear::Vector const & estimate, linear::Vector const & variance) const
    {
      stream_ << GetNorm(GetDiscrepancy(equation, estimate)) << " ";
    }

  private:
    std::ostream & stream_;
  };
}
