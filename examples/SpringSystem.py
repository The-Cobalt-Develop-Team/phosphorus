import numpy as np
import matplotlib.pyplot as plt
import subprocess
from math import sqrt, log10


def calc(step: float):
    subprocess.run(['SpringSystem.exe', str(step)])
    filename = "output.txt"
    data = []
    with open(filename, "r") as file:
        for line in file:
            data.append(tuple(map(float, line.split())))
    time, verlet_res, cpp_res = map(np.array, zip(*data))
    k, m, v0 = 1.0, 1.0, 1.0
    omega = sqrt(k / m)
    A = v0 / omega
    py_res = A * np.sin(omega * time)
    error = np.abs(verlet_res - cpp_res)
    max_error = np.max(error)
    print(f"log(Step): {log10(step):.3f}, log(Max error): {log10(max_error):.3f}")
    return time, verlet_res, cpp_res, py_res, error, max_error


def plot_error():
    step = np.array([0.1 ** i for i in range(1, 6)])
    errors = np.array([res[-1] for res in [calc(s) for s in step]])
    p_step = -np.log10(step)
    p_errors = -np.log10(errors)
    plt.plot(p_step, p_errors, label="Error", color="purple")
    plt.xlabel("-log(Step) size")
    plt.ylabel("-log(Max Error)")
    plt.title("Error vs Step Size")
    plt.grid()
    plt.savefig("spring_system_error_vs_step.png")
    plt.show()


def plot_figure():
    time, verlet_res, cpp_res, py_res, error, max_error = calc(0.01)

    plt.plot(time, py_res, label="Python", color="blue", linestyle="dashed")
    plt.plot(time, cpp_res, label="C++", color="red")
    plt.plot(time, verlet_res, label="Verlet", color="green")
    plt.xlabel("Time (s)")
    plt.ylabel("Displacement (m)")
    plt.title("Spring System Simulation")
    plt.legend()
    plt.grid()
    plt.savefig("spring_system_calc.png")
    plt.clf()

    plt.plot(time, error, label="Error", color="purple")
    plt.xlabel("Time (s)")
    plt.ylabel("Error (m)")
    plt.title("Error between C++ and Verlet methods")
    plt.legend()
    plt.grid()
    plt.savefig("spring_system_error.png")

    print(f"Max error: {max_error:.6f}")


if __name__ == "__main__":
    plot_error()
