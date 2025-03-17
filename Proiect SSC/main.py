import tkinter as tk
from tkinter import ttk
import subprocess
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

def write_to_file(filename, language, time_ns):
    with open(filename, "a") as file:
        file.write(f"Limbaj: {language}, timp: {time_ns} ns\n")

def show_results_window(sizes, c_results, cpp_results, selected_operation, java_results=None):
    results_window = tk.Toplevel(root)
    results_window.title("Rezultate in nanosecunde")
    results_window.geometry("450x650")

    frame = ttk.Frame(results_window)
    frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

    text_widget = tk.Text(frame, wrap=tk.WORD)
    text_widget.pack(fill=tk.BOTH, expand=True)

    text_widget.insert(tk.END, f"Rezultate Benchmark - {selected_operation}\n\n", "header")
    text_widget.tag_configure("header", font=("Arial", 12, "bold"))

    for i, size in enumerate(sizes):
        text_widget.insert(tk.END, f"\nDimensiune: {size}\n", "subheader")
        text_widget.insert(tk.END, f"C: {c_results[i]:.2f} ns\n")
        text_widget.insert(tk.END, f"C++: {cpp_results[i]:.2f} ns\n")
        if java_results is not None:
            text_widget.insert(tk.END, f"Java: {java_results[i]:.2f} ns\n")
        text_widget.insert(tk.END, "-" * 30 + "\n")

    text_widget.tag_configure("subheader", font=("Arial", 10, "bold"))
    text_widget.config(state=tk.DISABLED)

def generate_line_chart(c_results, cpp_results, java_results, selected_operation, sizes):
    fig, ax = plt.subplots(figsize=(10, 6))

    ax.plot(sizes, c_results, label='C', marker='o', linestyle='-', color='#0000FF')
    ax.plot(sizes, cpp_results, label='C++', marker='s', linestyle='--', color='#6666FF')
    ax.plot(sizes, java_results, label='Java', marker='^', linestyle='-.', color='#FF8000')

    ax.set_title(f'Comparatie performanta - {selected_operation}', fontsize=14)
    ax.set_xlabel('Dimensiune', fontsize=12)
    ax.set_ylabel('Rezultate (nanosecunde)', fontsize=12)
    ax.grid(True, linestyle='--', alpha=0.7)

    ax.legend()

    graph_window = tk.Toplevel(root)
    graph_window.title("Rezultate Benchmark - Grafic")
    graph_window.geometry("800x600")

    canvas = FigureCanvasTkAgg(fig, master=graph_window)
    canvas_widget = canvas.get_tk_widget()
    canvas_widget.pack(fill=tk.BOTH, expand=True)

    canvas.draw()

    show_results_window(sizes, c_results, cpp_results, selected_operation, java_results)

def generate_chart(c_result, cpp_result, selected_operation):
    fig, ax = plt.subplots(figsize=(10, 6))

    languages = ['C', 'C++']
    values = [c_result, cpp_result]

    bars = ax.bar(languages, values, color=['#0000FF', '#6666FF'], width=0.2)
    ax.set_title(f'Comparatie performanta limbaje de programare\nOperatie: {selected_operation}', fontsize=14)
    ax.set_xlabel('Limbaje de Programare', labelpad=4, fontsize=12)
    ax.set_ylabel('Rezultate (nanosecunde)', fontsize=12)

    for bar, value in zip(bars, values):
        ax.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), f'{value:.3f}',
                ha='center', va='bottom', fontsize=10)

    graph_window = tk.Toplevel(root)
    graph_window.title("Rezultate Benchmark - Grafic")
    graph_window.geometry("800x600")

    canvas = FigureCanvasTkAgg(fig, master=graph_window)
    canvas_widget = canvas.get_tk_widget()
    canvas_widget.pack(fill=tk.BOTH, expand=True)

    canvas.draw()

    show_results_window([1], [c_result], [cpp_result], selected_operation)

def generate_values(c_results, cpp_results, java_results, operation_code, sizes):
    for size in sizes:
        result = subprocess.run(["C:/UTCN/SSC/Proiect_C/main", operation_code, str(size)],
                              text=True, capture_output=True, check=True)
        output = result.stdout.strip()
        c_results.append(float(output))

        result = subprocess.run(["C:/UTCN/SSC/Proiect_C++/main", operation_code, str(size)],
                              text=True, capture_output=True, check=True)
        output = result.stdout.strip()
        cpp_results.append(float(output))

        result = subprocess.run(['java', '-cp', 'C:/UTCN/SSC/Proiect_Java/src', 'Main',
                               operation_code, str(size)],
                              text=True, capture_output=True, check=True)
        output = result.stdout.strip()
        java_results.append(float(output))

def process_results(c_results, cpp_results, java_results, selected_operation, sizes, filename, operation_type):
    for i in range(len(sizes)):
        with open(filename, "a") as file:
            file.write(f"Marime: {sizes[i]} {operation_type}\n")
            file.write(f"Limbaj: C, timp: {c_results[i]} ns\n")
            file.write(f"Limbaj: C++, timp: {cpp_results[i]} ns\n")
            file.write(f"Limbaj: Java, timp: {java_results[i]} ns\n\n")

    generate_line_chart(c_results, cpp_results, java_results, selected_operation, sizes)

def start_benchmark():
    c_results = []
    cpp_results = []
    java_results = []
    memory_sizes = [100, 500, 1000, 5000, 10000, 25000]
    thread_sizes = [1, 5, 10, 25, 50]
    filename = "benchmark_results.txt"

    selected_operation = choicebox.get()
    if selected_operation == "Alege o operatie":
        error_label.config(text="Selecteaza o operatie")
        return

    error_label.config(text="")
    operation_map = {
        "Alocare de memorie statica": "static_memory",
        "Alocare de memorie dinamica": "dynamic_memory",
        "Accesul la memorie": "memory_access",
        "Crearea unui thread": "create_thread",
        "Thread context switch": "context_switch",
        "Thread migration": "thread_migration"
    }

    operation_code = operation_map.get(selected_operation)
    with open(filename, "w") as file:
        file.write(f"Operatie: {selected_operation}\n\n")

    try:
        if operation_code in ["static_memory", "dynamic_memory", "memory_access"]:
            generate_values(c_results, cpp_results, java_results, operation_code, memory_sizes)
            process_results(c_results, cpp_results, java_results, selected_operation, memory_sizes,
                          filename, "elemente")

        elif operation_code in ["create_thread", "context_switch"]:
            generate_values(c_results, cpp_results, java_results, operation_code, thread_sizes)
            process_results(c_results, cpp_results, java_results, selected_operation, thread_sizes,
                          filename, "threaduri")

        else:
            result = subprocess.run(["C:/UTCN/SSC/Proiect_C/main", operation_code],
                                  text=True, capture_output=True, check=True)
            output = result.stdout.strip()
            c_result = float(output)

            result = subprocess.run(["C:/UTCN/SSC/Proiect_C++/main", operation_code],
                                  text=True, capture_output=True, check=True)
            output = result.stdout.strip()
            cpp_result = float(output)

            with open(filename, "a") as file:
                file.write("Migrare pe un nucleu\n")
            write_to_file(filename, "C", c_result)
            write_to_file(filename, "C++", cpp_result)

            generate_chart(c_result, cpp_result, selected_operation)

    except subprocess.CalledProcessError as e:
        error_label.config(text="A aparut o eroare la rularea programelor.")
        print(f"Eroare: {e}")
    except FileNotFoundError:
        error_label.config(text="Programul C nu a fost gasit.")
        print("Verifica daca programul este disponibil.")

root = tk.Tk()
root.title("Benchmark Operatii")
root.geometry("600x500")

title_label = tk.Label(root, text="Benchmark al operatiilor in C, C++ si Java",
                      font=("Arial", 20, "bold"))
title_label.pack(pady=(20, 40))

select_label = tk.Label(root, text="Selectare operatie", font=("Arial", 18))
select_label.pack(pady=(40, 5))

operations = [
    "Alocare de memorie statica",
    "Alocare de memorie dinamica",
    "Accesul la memorie",
    "Crearea unui thread",
    "Thread context switch",
    "Thread migration"
]

choicebox = ttk.Combobox(root, values=operations, state="readonly", font=("Arial", 14), width=30)
choicebox.set("Alege o operatie")
choicebox.pack(pady=(5, 40))

error_label = tk.Label(root, text="", font=("Arial", 18), fg="red")
error_label.pack(pady=(40, 10))

start_button = tk.Button(root, text="Start", command=start_benchmark, font=("Arial", 16))
start_button.pack(pady=10)

root.mainloop()