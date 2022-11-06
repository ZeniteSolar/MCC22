def convert_ltspice_step_simulation_data_to_csv(datasets: list, output_filename: str, input_separator: str = '\t', output_separator: str = ',', expoents: dict = {
    "g": 'e9',
    "meg": 'e6',
    "k": 'e3',
    "m": 'e-3',
    "u": 'e-6',
    "n": 'e-9',
}):
    """
    Sample of input file:
    time	V(eff)	V(pcc)	V(pci)	V(pco)	V(pdiode)	V(pin)	V(pli)	V(plo)	V(pmos)	V(pout)
    Step Information: Li=10u Cc=1u Lo=10u  (Run: 1/36)
    0.000000000000000e+000	9.112822e-001	2.973614e+001	-4.158429e+000	-1.784582e+001	5.715999e+000	3.396873e+002	8.995298e+000	6.411624e+000	-1.#IND00e+000	3.039183e+002

    Sample of output file:
    fsw,Li,Cc,Lo,time,V(eff),V(pcc),V(pci),V(pco),V(pdiode),V(pin),V(pli),V(plo),V(pmos),V(pout)
    40e3,10e-6,1e-6,10e-6,0.000000000000000e+000,9.112822e-001,2.973614e+001,-4.158429e+000,-1.784582e+001,5.715999e+000,3.396873e+002,8.995298e+000,6.411624e+000,-1.#IND00e+000,3.039183e+002
    """

    output_lines = []
    header = None
    variables_cols = []
    indexes_dict = {}
    for dataset in datasets:

        filename = dataset["file"]
        file = open(filename, 'r')
        # lines = file.readlines()

        for (k, line) in enumerate(file):
            if k == 0:
                variables_cols = line[:-1].split(input_separator)

            if line.startswith("Step Information:"):
                # found the "Step Information: Li=10u Cc=1u Lo=10u  (Run: 1/36)"

                # Start the new dataset
                cases_raw = line.split('Step Information: ')[1].split(
                    '  (Run')[0].split(' ')

                indexes_dict = {}
                for variable in cases_raw:
                    key, value = variable.split('=')
                    value = value.lower()
                    has_expoent = [key in value for key in expoents.keys()]
                    if True in has_expoent:
                        value = value[:-1] + expoents[value[-1]]
                    indexes_dict[key] = value

                if header is None:
                    # Create the header and put it in the beggining of the file
                    index_cols = list(dataset.keys())[
                        :-1] + list(indexes_dict.keys())
                    header = [*index_cols, *variables_cols]
                    output_lines.append(output_separator.join(header) + '\n')

            elif k > 2:
                # Save the indexes values
                data_cols = list(dataset.values())[
                    :-1] + list(indexes_dict.values()) + line[:-1].split('\t')
                data_line = output_separator.join(data_cols) + '\n'
                output_lines.append(data_line)

    out_file = open(output_filename, 'w', encoding='UTF-8')
    out_file.writelines(output_lines)
    out_file.close()


def example():
    dataset_filename = "simulation_optimization_dataset.csv"
    datasets = [
        {"fsw": '40e3', "file": "../simulations_ltspice/1_cuk_40khz_optimizing_components.txt"}
    ]

    convert_ltspice_step_simulation_data_to_csv(
        datasets=datasets, output_filename=dataset_filename)

    import pandas as pd

    dataset_filename = "simulation_optimization_dataset.csv"
    df = pd.read_csv(dataset_filename)
    print(df)


if __name__ == "__main__":
    example()
