import numpy as np
from dataclasses import dataclass
import pandas as pd


@dataclass
class PanelModelParams():
    Voc: float
    Isc: float
    G: float


class FakePanelModel(PanelModelParams):
    def generic_fake_panel_model(self, Vpv: float) -> float:
        Ipv = 0
        if Vpv < self.Voc:
            Ipv = max(0, 0.555 * self.Isc *
                      (self.G / 1000.0) *
                      (1.0 + ((Vpv - self.Voc + 1.0) / (Vpv - self.Voc - 1.0))))
        return Ipv

    def data_as_pandas(self, panel_voltage: np.array) -> pd.DataFrame:
        panel_current = np.array([self.generic_fake_panel_model(
            v) for v in panel_voltage], like=panel_voltage)
        return pd.DataFrame({'V': panel_voltage, 'I': panel_current, 'P': panel_voltage * panel_current})
