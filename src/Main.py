import logging
import MTM1M3TSController
import Model
import Context
import Controller
import Threads
import Commands
import ThermalScannerClient

def run():
    logging.basicConfig(format = "%(asctime)-15s %(threadName)-16s %(name)-10s %(levelname)-8s %(message)s")
    logging.getLogger("Command").setLevel(logging.INFO)
    logging.getLogger("Context").setLevel(logging.INFO)
    logging.getLogger("Controller").setLevel(logging.INFO)
    logging.getLogger("Model").setLevel(logging.INFO)
    logging.getLogger("State").setLevel(logging.INFO)
    logging.getLogger("Thread").setLevel(logging.DEBUG)
    logging.getLogger("Main").setLevel(logging.DEBUG)
    log = logging.getLogger("Main")
    log.info("Starting MTM1M3TS application.")
    log.info("Initializing MTM1M3TS SAL interface.")
    sal = MTM1M3TSController.MTM1M3TSController()
    log.info("Initializing MTM1M3TS model.")
    model = Model.Model(sal)
    log.info("Initializing MTM1M3TS context.")
    context = Context.Context(sal, model)
    log.info("Initializing MTM1M3TS controller.")
    controller = Controller.Controller(context)
    log.info("Initializing MTM1M3TS controller thread.")
    controllerThread = Threads.ControllerThread(controller, 0.001)
    log.info("Initializing MTM1M3TS subscriber thread.")
    subscriberThread = Threads.SubscriberThread(sal, controller, 0.001)
    log.info("Initializing MTM1M3TS outer loop thread.")
    outerLoopThread = Threads.OuterLoopThread(controller, 0.050)
    log.info("Initializing Thermal Scanner 1.")
    thermalScannerClient1 = ThermalScannerClient.ThermalScannerClient("0.0.0.0", 10001)
    log.info("Initializing Thermal Scanner 2.")
    thermalScannerClient2 = ThermalScannerClient.ThermalScannerClient("0.0.0.0", 10002)
    log.info("Initializing Thermal Scanner 3.")
    thermalScannerClient3 = ThermalScannerClient.ThermalScannerClient("0.0.0.0", 10003)
    log.info("Initializing Thermal Scanner 4.")
    thermalScannerClient4 = ThermalScannerClient.ThermalScannerClient("0.0.0.0", 10004)
    log.info("Initializing MTM1M3TS thermal scanner thread.")
    thermalScannerThread = Threads.ThermalScannerThread(controller, thermalScannerClient1, thermalScannerClient2, thermalScannerClient3, thermalScannerClient4)
    log.info("Adding BootCommand for OfflineState to StandbyState transition.")
    controller.enqueue(Commands.BootCommand())
    log.info("Starting MTM1M3TS controller thread.")
    controllerThread.start()
    log.info("Starting MTM1M3TS subscriber thread.")
    subscriberThread.start()
    log.info("Starting MTM1M3TS outer loop thread.")
    outerLoopThread.start()
    log.info("Starting MTM1M3TS thermal scanner thread.")
    thermalScannerThread.start()
    log.info("MTM1M3TS is now running and waiting for shutdown.")
    model.waitForShutdown()
    log.info("MTM1M3TS has a shutdown request.")
    log.info("Stopping MTM1M3TS controller thread.")
    controllerThread.stop()
    controllerThread.join()
    log.info("Stopping MTM1M3TS subscriber thread.")
    subscriberThread.stop()
    subscriberThread.join()
    log.info("Stopping MTM1M3TS outer loop thread.")
    outerLoopThread.stop()
    outerLoopThread.join()
    log.info("Stopping MTM1M3TS thermal scanner thread.")
    thermalScannerThread.stop()
    log.info("Shutting down Thermal Scanner 1.")
    thermalScannerClient1.prepareShutdown()
    log.info("Shutting down Thermal Scanner 2.")
    thermalScannerClient2.prepareShutdown()
    log.info("Shutting down Thermal Scanner 3.")
    thermalScannerClient3.prepareShutdown()
    log.info("Shutting down Thermal Scanner 4.")
    thermalScannerClient4.prepareShutdown()
    log.info("Joining thermal scanner thread.")
    thermalScannerThread.join()
    log.info("Closing thermal scanner 1 client.")
    thermalScannerClient1.close()
    log.info("Closing thermal scanner 2 client.")
    thermalScannerClient2.close()
    log.info("Closing thermal scanner 3 client.")
    thermalScannerClient3.close()
    log.info("Closing thermal scanner 4 client.")
    thermalScannerClient4.close()
    log.info("Closing MTM1M3TS SAL interface.")
    sal.close()