/**
 *  \file IMP/isd/GaussianProcessInterpolationRestraint.h
 *  \brief Restraint and ScoreState for GaussianProcessInterpolation
 *
 *  Copyright 2007-2022 IMP Inventors. All rights reserved.
 */

#ifndef IMPISD_GAUSSIAN_PROCESS_INTERPOLATION_RESTRAINT_H
#define IMPISD_GAUSSIAN_PROCESS_INTERPOLATION_RESTRAINT_H

#include <IMP/isd/isd_config.h>
#include <IMP/macros.h>
#include <boost/scoped_ptr.hpp>
#include <IMP/Restraint.h>
#include <IMP/isd/GaussianProcessInterpolation.h>
#include <IMP/isd/MultivariateFNormalSufficient.h>
#include <IMP/Pointer.h>
#include <Eigen/Dense>

#include <IMP/ScoreState.h>

IMPISD_BEGIN_NAMESPACE

class GaussianProcessInterpolationScoreState;

//! gaussian process restraint
/* the restraint is a multivariate normal distribution on the vector of
* observations with mean and standard deviation given by the posterior of the
* gaussian process.
*/
class IMPISDEXPORT GaussianProcessInterpolationRestraint
    : public Restraint {
 private:
  // checks and makes necessary updates
  void update_mean_and_covariance();
  IMP::PointerMember<GaussianProcessInterpolation> gpi_;
  IMP::PointerMember<MultivariateFNormalSufficient> mvn_;
  IMP::PointerMember<GaussianProcessInterpolationScoreState> ss_;
  // number of observation points
  unsigned M_;

  void create_score_state();

 public:
  /** This is a restraint on other restraints. It first constructs the
     necessary vectors from GaussianProcessInterpolation, then creates a
     multivariate normal distribution around it. Upon evaluation, it
     checks if parameters have changed, reconstructs the matrix if
     necessary, changes the DA weight and passes it to the functions. */
  GaussianProcessInterpolationRestraint(Model *m,
                                        GaussianProcessInterpolation *gpi);

  /** To call this, you need to update the scorestate before.
      calling model.evaluate(False) is enough. */
  double get_probability() const { return mvn_->density(); }

  //! Use conjugate gradients when possible (default false)
  void set_use_cg(bool use, double tol) { mvn_->set_use_cg(use, tol); }

  //! Get minus log normalization and minus exponent separately
  double get_minus_log_normalization() const;
  double get_minus_exponent() const;

  //! Get hessian of the minus log likelihood
  Eigen::MatrixXd get_hessian() const;

  //! Get log determinant of hessian
  double get_logdet_hessian() const;

  //! call this one from Python
  FloatsList get_hessian(bool unused) const;

 public:
  double unprotected_evaluate(IMP::DerivativeAccumulator *accum) const
      override;
  IMP::ModelObjectsTemp do_get_inputs() const override;
  IMP_OBJECT_METHODS(GaussianProcessInterpolationRestraint);

  // to allow the scorestate to get the restraint's objects
  friend class GaussianProcessInterpolationScoreState;
};

#if !defined(IMP_DOXYGEN) && !defined(SWIG)
class IMPISDEXPORT GaussianProcessInterpolationScoreState : public ScoreState {
 private:
  IMP::WeakPointer<GaussianProcessInterpolationRestraint> gpir_;

 private:
  GaussianProcessInterpolationScoreState(
      GaussianProcessInterpolationRestraint *gpir)
      : ScoreState(gpir->get_model(),
                   "GaussianProcessInterpolationScoreState%1%"),
        gpir_(gpir) {}

 public:
  // only the GPIR can create this and add it to the model
  friend class GaussianProcessInterpolationRestraint;
  virtual void do_before_evaluate() override;
  virtual void do_after_evaluate(DerivativeAccumulator *da) override;
  virtual ModelObjectsTemp do_get_inputs() const override;
  virtual ModelObjectsTemp do_get_outputs() const override;
  IMP_OBJECT_METHODS(GaussianProcessInterpolationScoreState);
};
#endif

IMPISD_END_NAMESPACE

#endif /* IMPISD_GAUSSIAN_PROCESS_INTERPOLATION_RESTRAINT_H */
