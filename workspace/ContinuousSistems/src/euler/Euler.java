package euler;

import java.math.BigDecimal;

/**
 * @author Elisa Boselli
 *
 */
public abstract class Euler {

  protected String derivative;

  /**
   * Gets the next function's value using the Euler's technique. It is assumed that the first value
   * of the list will be the one corresponding to the current function, while the rest will be of
   * the other related functions.
   * 
   * @param previousValue
   *          - Previous function's value.
   * @param step
   *          - Euler's step.
   * @return (BigDecimal) Next function's value.
   */
  public BigDecimal applyEuler(BigDecimal step, BigDecimal... previousValues) {
    return (previousValues[0]).add(step.multiply(applyDerivative(previousValues)));
  }

  /**
   * Apply the function's derivative. It is assumed that the first value of the list will be the one
   * corresponding to the current function, while the rest will be of the other related functions.
   * 
   * @param previousValue
   *          - Previous function's value.
   * @return (BigDecimal) Derivative application.
   */
  protected abstract BigDecimal applyDerivative(BigDecimal... previousValues);

}
