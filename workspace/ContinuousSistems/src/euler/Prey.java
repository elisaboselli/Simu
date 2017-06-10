package euler;

import java.math.BigDecimal;

/**
 * @author Elisa Boselli
 *
 */
public final class Prey extends Euler {

  private BigDecimal growthRate;
  private BigDecimal mortalityRate;

  /**
   * Constructor of the class - returns a Prey object.
   * 
   * @param growth
   *          - Prey's growth rate in the absence of predators.
   * @param mortality
   *          - Prey's mortality rate.
   */
  public Prey(BigDecimal growth, BigDecimal mortality) {
    this.growthRate = growth;
    this.mortalityRate = mortality;
    if (!repOk()) {
      throw new IllegalArgumentException();
    }
  }

  /**
   * Apply the function's derivative. It is assumed that the first value of the list will be the one
   * corresponding to the current function, while the rest will be of the other related functions.
   * 
   * @param previousValue
   *          - Previous function's value.
   * @return (BigDecimal) Derivative application.
   */
  @Override
  protected BigDecimal applyDerivative(BigDecimal... previousValues) {
    if (previousValues.length == 2) {
      BigDecimal prey = previousValues[0];
      BigDecimal predator = previousValues[1];
      BigDecimal growth = growthRate.multiply(prey);
      BigDecimal mortality = mortalityRate.multiply(prey).multiply(predator);
      BigDecimal result = growth.subtract(mortality);
      return result;
    } else {
      throw new IllegalArgumentException();
    }
  }

  /**
   * Check if all the parameters are not negative.
   * 
   * @return (Boolean) True if any parameter is negative, otherwise false.
   */
  private boolean repOk() {
    BigDecimal zero = new BigDecimal("0");
    return (growthRate.compareTo(zero) > 0) && (mortalityRate.compareTo(zero) > 0);
  }

}
