# Scientific Model

## Scope

This document defines the physical model and coordinate conventions used by
`dp-cuda`.

The system is an ideal double pendulum composed of two point masses connected
by massless, rigid rods and constrained to move in a vertical plane.

Effects such as friction, air resistance, elastic deformation, external forcing,
and collisions are not included.

## Coordinate conventions

- `theta1` is the absolute angle of the first rod.
- `theta2` is the absolute angle of the second rod; it is not measured relative
  to the first rod.
- Both angles are measured from the downward vertical direction.
- Positive angles correspond to counterclockwise rotation.
- `omega1` and `omega2` are the corresponding angular velocities.
- The stable hanging configuration is `theta1 = theta2 = 0`.

Angles are not automatically wrapped to a fixed interval during integration.
This allows the state to retain information about complete rotations.

## Units

The model uses SI units throughout:

| Quantity | Symbol | Unit |
| --- | --- | --- |
| Angle | `theta1`, `theta2` | rad |
| Angular velocity | `omega1`, `omega2` | rad/s |
| Angular acceleration | `alpha1`, `alpha2` | rad/s^2 |
| Mass | `m1`, `m2` | kg |
| Length | `l1`, `l2` | m |
| Gravitational acceleration | `g` | m/s^2 |
| Energy | `T`, `V`, `E` | J |

## State

The state of one double pendulum is represented by:

```text
y = (theta1, theta2, omega1, omega2)
```

The angular velocities are defined as:

```text
omega1 = d(theta1) / dt
omega2 = d(theta2) / dt
```

## Physical parameters

The physical parameters are:

| Parameter | Meaning | Default |
| --- | --- | --- |
| `m1` | Mass attached to the first rod | `1.0 kg` |
| `m2` | Mass attached to the second rod | `1.0 kg` |
| `l1` | Length of the first rod | `1.0 m` |
| `l2` | Length of the second rod | `1.0 m` |
| `g` | Magnitude of gravitational acceleration | `9.80665 m/s^2` |

Masses and lengths must be finite and strictly positive. The gravitational
acceleration is treated as a magnitude and must be finite and non-negative.
Setting `g = 0` represents a system without gravity.

## Positions

The first pivot is the origin. The horizontal axis points to the right and the
vertical axis points upward. With these conventions, the positions of the two
masses are:

```text
x1 =  l1 * sin(theta1)
y1 = -l1 * cos(theta1)

x2 = x1 + l2 * sin(theta2)
y2 = y1 - l2 * cos(theta2)
```

## Equations of motion

Define the angular difference:

```text
delta = theta1 - theta2
```

The angular accelerations are:

```text
alpha1 = d(omega1) / dt
alpha2 = d(omega2) / dt
```

They are obtained from the following coupled equations:

```text
(m1 + m2) * l1 * alpha1
    + m2 * l2 * cos(delta) * alpha2
    = -m2 * l2 * omega2^2 * sin(delta)
      - (m1 + m2) * g * sin(theta1)

l1 * cos(delta) * alpha1
    + l2 * alpha2
    = l1 * omega1^2 * sin(delta)
      - g * sin(theta2)
```

The simulator integrates the equivalent first-order system:

```text
d(theta1) / dt = omega1
d(theta2) / dt = omega2
d(omega1) / dt = alpha1
d(omega2) / dt = alpha2
```

The implementation may use algebraically equivalent explicit expressions for
`alpha1` and `alpha2`. The coupled equations above remain the reference model.

## Energy

The kinetic energy is:

```text
T = 0.5 * (m1 + m2) * l1^2 * omega1^2
    + 0.5 * m2 * l2^2 * omega2^2
    + m2 * l1 * l2 * omega1 * omega2 * cos(theta1 - theta2)
```

The zero of potential energy is chosen at the stable hanging configuration. The
potential energy is:

```text
V = (m1 + m2) * g * l1 * (1 - cos(theta1))
    + m2 * g * l2 * (1 - cos(theta2))
```

The total mechanical energy is:

```text
E = T + V
```

For the ideal continuous model, total energy is conserved. A numerical
method can only approximate this conservation law.

## Chaotic behavior

The double pendulum can exhibit sensitive dependence on initial conditions.
Consequently, very small differences in the initial state can grow rapidly over
time and eventually produce visibly different trajectories. This is an
intrinsic property of the physical system.

## Assumptions and limitations

- The rods are rigid and massless.
- The joints are frictionless.
- The masses are point particles.
- Gravity is uniform and constant.
- Motion is restricted to a two-dimensional plane.
- No damping or external forces are present.
