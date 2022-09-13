from enum import Enum
from dataclasses import dataclass, asdict
from typing_extensions import Self
from pint import UnitRegistry
units = UnitRegistry()


class ComponentType(Enum):
    RESISTOR = 0
    INDUCTOR = 1
    CAPACITOR = 2
    DIODE = 3
    MOSFET = 4


@dataclass
class Waveform():
    avg: float
    rms: float
    pkpk: float
    max: float
    min: float


@dataclass
class IdealComponent():
    value: float
    type: ComponentType
    I: Waveform
    V: Waveform


@dataclass
class OperationPoint():
    output_voltage: float
    input_voltage: float
    duty_cycle: float


@dataclass
class DesignConstraints():
    switching_frequency: float
    output_power: float
    input_inductor_ripple: float
    output_inductor_ripple: float
    coupling_capacitor_ripple: float
    output_capacitor_ripple: float


class IdealCukFactory():
    def input_inductor_factory(operation_point: OperationPoint, design_constraints: DesignConstraints):
        # Input
        f = design_constraints.switching_frequency
        P_o = design_constraints.output_power
        Ripple = design_constraints.input_inductor_ripple
        V_o = operation_point.output_voltage
        V_i = operation_point.input_voltage
        D = operation_point.duty_cycle

        # Compute
        I_L_avg = (D / (1 - D)) * (P_o / V_o)
        I_L_pkpk = I_L_avg * Ripple
        I_L_rms = (I_L_avg**2 - ((I_L_pkpk**2) / 12))**(1/2)
        I_L_max = I_L_avg * (1 + (Ripple / 2))
        I_L_min = I_L_avg * (1 - (Ripple / 2))
        L = V_o * (1 - D) / (f * I_L_pkpk)

        # Output
        return IdealComponent(
            value=L,
            type=ComponentType.INDUCTOR,
            I=Waveform(
                avg=I_L_avg,
                rms=I_L_rms,
                pkpk=I_L_pkpk,
                max=I_L_max,
                min=I_L_min,
            ),
            V=None
        )

    def output_inductor_factory(operation_point: OperationPoint, design_constraints: DesignConstraints):
        # Input
        f = design_constraints.switching_frequency
        P_o = design_constraints.output_power
        Ripple = design_constraints.output_inductor_ripple
        V_o = operation_point.output_voltage
        V_i = operation_point.input_voltage
        D = operation_point.duty_cycle

        # Compute
        I_L_avg = (P_o / V_o)
        I_L_pkpk = I_L_avg * Ripple
        I_L_rms = (I_L_avg**2 - ((I_L_pkpk**2) / 12))**(1/2)
        I_L_max = I_L_avg * (1 + (Ripple / 2))
        I_L_min = I_L_avg * (1 - (Ripple / 2))
        L = V_i * D / (f * I_L_pkpk)

        # Output
        return IdealComponent(
            value=L,
            type=ComponentType.INDUCTOR,
            I=Waveform(
                avg=I_L_avg,
                rms=I_L_rms,
                pkpk=I_L_pkpk,
                max=I_L_max,
                min=I_L_min,
            ),
            V=None
        )

    def coupling_capacitor_factory(operation_point: OperationPoint, design_constraints: DesignConstraints, input_inductor: IdealComponent):
        # Input
        f = design_constraints.switching_frequency
        P_o = design_constraints.output_power
        Ripple = design_constraints.coupling_capacitor_ripple
        V_o = operation_point.output_voltage
        V_i = operation_point.input_voltage
        D = operation_point.duty_cycle
        I_Li_avg = input_inductor.I.avg
        I_Li_pkpk = input_inductor.I.pkpk

        # Compute
        I_o = P_o / V_o
        V_C_avg = (V_o / D)
        V_C_pkpk = V_C_avg * Ripple
        V_C_max = V_C_avg * (1 + (Ripple / 2))
        V_C_min = V_C_avg * (1 - (Ripple / 2))
        I_C_rms = I_o * ((1-D) / D)**(1/2)
        C_esr = V_C_pkpk / I_Li_pkpk
        C = I_Li_avg * (1 - D) / (f * V_C_pkpk)

        # Output
        return IdealComponent(
            value=C,
            type=ComponentType.CAPACITOR,
            I=Waveform(
                avg=0,
                rms=I_C_rms,
                pkpk=0,
                max=0,
                min=0,
            ),
            V=Waveform(
                avg=V_C_avg,
                rms=0,
                pkpk=V_C_pkpk,
                max=V_C_max,
                min=V_C_min,
            )
        )

    def output_capacitor_factory(operation_point: OperationPoint, design_constraints: DesignConstraints, output_inductor: IdealComponent):
        # Input
        f = design_constraints.switching_frequency
        P_o = design_constraints.output_power
        Ripple = design_constraints.output_capacitor_ripple
        V_o = operation_point.output_voltage
        V_i = operation_point.input_voltage
        D = operation_point.duty_cycle
        Lo = output_inductor.value
        I_Lo_pkpk = output_inductor.I.pkpk

        # Compute
        V_C_avg = V_o
        V_C_pkpk = V_C_avg * Ripple
        V_C_max = V_C_avg * (1 + (Ripple / 2))
        V_C_min = V_C_avg * (1 - (Ripple / 2))
        I_C_rms = I_Lo_pkpk / (2 * (3)**(1/2))
        C_esr = V_C_pkpk / I_Lo_pkpk
        C = (D * V_i) / (8 * f**2 * Lo * V_C_pkpk)

        # Output
        return IdealComponent(
            value=C,
            type=ComponentType.CAPACITOR,
            I=Waveform(
                avg=0,
                rms=I_C_rms,
                pkpk=0,
                max=0,
                min=0,
            ),
            V=Waveform(
                avg=V_C_avg,
                rms=0,
                pkpk=V_C_pkpk,
                max=V_C_max,
                min=V_C_min,
            )
        )


@dataclass(init=False)
class IdealCuk():
    input_inductor: IdealComponent
    output_inductor: IdealComponent
    coupling_capacitor: IdealComponent
    output_capacitor: IdealComponent
    operation_point: OperationPoint
    design_constraints: DesignConstraints

    def __init__(self, operation_point: OperationPoint, design_constraints: DesignConstraints) -> Self:
        self.operation_point = operation_point
        self.design_constraints = design_constraints
        self.input_inductor = IdealCukFactory.input_inductor_factory(
            operation_point, design_constraints)
        self.output_inductor = IdealCukFactory.output_inductor_factory(
            operation_point, design_constraints)
        self.coupling_capacitor = IdealCukFactory.coupling_capacitor_factory(
            operation_point, design_constraints, self.input_inductor)
        self.output_capacitor = IdealCukFactory.output_capacitor_factory(
            operation_point, design_constraints, self.output_inductor)

    # @classmethod
    # def from_components(self, input_inductor_value: float,
    #                     output_inductor_value: float,
    #                     coupling_capacitor_value: float,
    #                     output_capacitor_value: float) -> Self:
    #     self.input_inductor = IdealComponent(
    #         value=input_inductor_value,
    #         type=ComponentType.INDUCTOR,
    #     )
    #     self.output_inductor = IdealComponent(
    #         value=output_inductor_value,
    #         type=ComponentType.INDUCTOR,
    #     )
    #     self.coupling_capacitor = IdealComponent(
    #         value=coupling_capacitor_value,
    #         type=ComponentType.CAPACITOR,
    #     )
    #     self.output_capacitor = IdealComponent(
    #         value=output_capacitor_value,
    #         type=ComponentType.CAPACITOR,
    #     )
    #     self.operation_point = None
    #     self.design_constraints = None


# def mosfet(self, P_o, V_o, f, D, I_Li, I_Lo, V_Cc, S_rds_on, S_t_sr, s_t_sf):
#     V_S = V_Cc
#     I_S = I_Li + I_Lo
#     I_S_rms = (P_o / V_o) * ((1 - D)**(1/2)) / (1 - D)
#     I_S_avg = I_S / 2
#     P_S_cond = S_rds_on * I_S_rms**2 * D
#     P_S_sw = I_S * V_S * f * ((S_t_sr + s_t_sf) / 2)
#     P_S = P_S_cond + P_S_sw
#     return (V_S,
#             I_S,
#             I_S_rms,
#             I_S_avg,
#             P_S_cond,
#             P_S_sw,
#             P_S)

# def diode(self, P_o, V_o, f, D, I_Li, I_Lo, V_Cc, D_V_to, D_r_T, D_Qrr):
#     V_D = V_Cc
#     I_D = I_Li + I_Lo
#     I_D_rms = (P_o / V_o) * ((D)**(1/2)) / (1 - D)
#     I_D_avg = I_D / 2
#     P_D_cond = D_V_to * I_D_avg + D_r_T * I_D_rms**2
#     P_D_sw = D_Qrr * V_D * f
#     P_D = P_D_cond + P_D_sw
#     return (V_D,
#             I_D,
#             I_D_rms,
#             I_D_avg,
#             P_D_cond,
#             P_D_sw,
#             P_D)
