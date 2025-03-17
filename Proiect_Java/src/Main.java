import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Main {
    static final int nr_of_tests = 100;
    static int size = 0;

    static Lock lock = new ReentrantLock();
    static AtomicBoolean ready = new AtomicBoolean(false);

    public static void measureStaticAllocation() {
        double time = 0.0;

        for (int i = 0; i < nr_of_tests; i++) {
            double startTime = System.nanoTime();

            int[] array = new int[size];

            double finalTime = System.nanoTime();
            time += finalTime - startTime;
        }

        time /= nr_of_tests;
//        System.out.printf("Static allocation time (ns): %.3f\n", time);
        System.out.printf("%.3f", time);
    }

    public static void measureDynamicAllocation() {
        double time = 0.0;

        for (int i = 0; i < nr_of_tests; i++) {
            double startTime = System.nanoTime();

            Integer[] array = new Integer[size];

            double finalTime = System.nanoTime();
            time += finalTime - startTime;
        }

        time /= nr_of_tests;
//        System.out.printf("Dynamic allocation time (ns): %.3f\n", time);
        System.out.printf("%.3f", time);
    }

    public static void measureAccess() {
        double time = 0.0;
        int x;
        int[] array = new int[size];
        for (int i = 0; i < size; i++) {
            array[i] = i;
        }


        for (int i = 0; i < nr_of_tests; i++) {
            double startTime = System.nanoTime();

            for (int j = 0; j < size; j++) {
                x = array[j];
            }

            double finalTime = System.nanoTime();
            time += finalTime - startTime;
        }

        time /= nr_of_tests;
//        System.out.printf("Memory access time (ns): %.3f\n", time);
        System.out.printf("%.3f", time);
    }

    public static void createThread() {
        double time = 0.0;

        for (int i = 0; i < nr_of_tests; i++) {


            double startTime = System.nanoTime();

            Thread[] threads = new Thread[size];
            for (int j = 0; j < size; j++) {
                threads[j] = new Thread(() -> {});
                threads[j].start();
            }
            
            double finalTime = System.nanoTime();

            for (int j = 0; j < size; j++) {
                try {
                    threads[j].join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            time += finalTime - startTime;
        }

        time /= nr_of_tests;
//        System.out.printf("Thread creation time (ns): %.3f\n", time);
        System.out.printf("%.3f", time);
    }

    public static void threadFunctionSwitch(AtomicBoolean flag, double[] switchTimes, int index) {
        while (!flag.get()) {
        }

        long start = System.nanoTime();

        for (int i = 0; i < 100; i++) {
            lock.lock();
            lock.unlock();
        }

        long end = System.nanoTime();
        switchTimes[index] = (double) (end - start) / 100;
    }

    public static void contextSwitch() {
        double totalSwitchTime = 0.0;

        for (int test = 0; test < nr_of_tests; test++) {
            Thread[] threads = new Thread[size];
            double[] switchTimes = new double[size];

            ready.set(false);

            for (int i = 0; i < size; i++) {
                int index = i;
                threads[i] = new Thread(() -> threadFunctionSwitch(ready, switchTimes, index));
            }

            for (Thread thread : threads) {
                thread.start();
            }

            ready.set(true);

            for (Thread thread : threads) {
                try {
                    thread.join();
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    System.err.println("Thread interrupted: " + e.getMessage());
                }
            }

            for (double switchTime : switchTimes) {
                totalSwitchTime += switchTime;
            }
        }

        double averageSwitchTime = totalSwitchTime / nr_of_tests;

//        System.out.printf("Thread context switch time (ns): %.3f%n", averageSwitchTime);
        System.out.printf("%.3f", averageSwitchTime);
    }

    public static void main(String[] args) {
        if (args.length == 2) {
            size = Integer.parseInt(args[1]);
        }

        if(args[0].equals("static_memory")){
            measureStaticAllocation();
        }
        else if(args[0].equals("dynamic_memory")){
            measureDynamicAllocation();
        }
        else if(args[0].equals("memory_access")){
            measureAccess();
        }
        else if(args[0].equals("create_thread")){
            createThread();
        }
        else if(args[0].equals("context_switch")){
            contextSwitch();
        }
        else{
            System.out.printf("Eroare: Operatie necunoscuta '%s'.\n\n", args[0]);
        }
    }
}