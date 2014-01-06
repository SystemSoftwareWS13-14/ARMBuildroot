# Design - buf_threaded

Das Modul buf_threaded simuliert mithilfe eines asynchronen threadsicheren Buffers externe Hardware.
Der read/write Zugriff auf diesen (FIFO)-Buffer soll hierbei in kernel Threads erfolgen. In dieser Implementierung
werden direkt eigene Threads verwendet (keine work_queues oder sonstiges).

## Inhalt
1. Buffer
2. Modul
 1. Threads
3. Datenfluss
 1. Unterschied read/write
4. Synchronisierung
5. Kritik

## Buffer
Der Buffer wurde in einer separaten Datei implementiert. Im Unterschied zum Buffer aus der Aufgabe
"Zugriffsmodi" verwendet dieser einen Mutex, um den gleichzeitigen Zugriff durch mehrere Threads zu verhindern.
Dadurch wird immer die Konsistenz des Buffers sichergestellt.

Der folgende Codeausschnitt zeigt die read Funktion:
```C
mutex_lock(&buf->buffer_mutex);
toRead = min(byte, buf->byteCount);
for (i = 0; i < toRead; ++i) {
        out[i] = buf->data[buf->index];
        buf->index = (buf->index + 1) % buf->size;
}
buf->byteCount -= toRead;
mutex_unlock(&buf->buffer_mutex);
```

Vor dem Schreibzugriff auf den Buffer muss der Mutex gelockt werden. Erst nach Abschluss des Lesevorgangs wir der Mutex wieder freigegeben. Alle anderen Funktionen wurden analog implementiert. 

## Modul
Im Kernel Modul buf_threaded sind die read und write Threads implementiert. Jeder read oder write Zugriff
greift auf den gemeinsamen globalen Buffer zu.

Beim blockierenden Zugriff wird die Bedingung "Buffer-voll" bzw. "Buffer-leer" getestet. Ist der Buffer voll bzw. leer
werden die jeweiligen Threads in die wait_queue eingefügt. Zum Synchronisierung beim wake_up der Tasks in den wait_queues wird ein globaler Mutex verwendet.

```C
static buffer dev_buf;
static wait_queue_head_t read_wait_queue;
static wait_queue_head_t write_wait_queue;
DEFINE_MUTEX(mutex)
```

### Threads
Die jeweiligen read und write Threads bekommen als Parameter beim Start jeweils eine Datenstruktur vom Typ **thread_data_t** übergeben.

```C
typedef struct {
        char *r_buff; // Read
        const char __user *w_buff; // Write
        size_t size;
        struct completion *compl;
} thread_data_t;
```

Diese Struktur enthält Pointer auf die zu lesenden/schreibenden Buffer sowie deren Größe. Die Struktur **struct completion** wird verwendet, um der erzeugenden Task das Ende dieses Threads zu signalisieren.
Aufgabe der Threads ist es, mithilfe eines zufälligen Sleeps die langsamere Hardware zu simulieren und anschließend
in den Buffer zu schreiben/von dem Buffer zu lesen. Danach wird die Completion-Variable **compl** gesetzt und auf
ein kthread_stop des erzeugenden Threads gewartet. Nach Rückgabe des Rückgabewertes ist der Thread beendet.

## Datenfluss
Bei read oder write Zugriffen auf die dazugehörige Gerätedatei werden die Funktionen **read** bzw. **write** aufgerufen.
Da beide nahezu identisch sind, wird hier nur auf die write Funktion eingegangen:

* Zu Beginn wird zwischen blockierendem Zugriff und nicht blockierendem Zugriff unterschieden (O_NONBLOCK Flag).
 * Nicht blockierend:
   Hier führt die Funktion ein **return** aus, wenn der Buffer bereits voll ist.
 * Blockierend:
   Ist der Buffer voll, blockiert der Thread in einer wait_queue.
* Ist der Buffer nicht voll, kann der jeweilige Thread erzeugt und gestartet werden. Anschließend wird auf Beendigung   
  des gestarteten Threads gewartet. Ist dieser mit dem write Vorgang fertig, wird der return-Wert abgefragt.
  Vor dem zurückgegeben des return-Werts werden ggf. weitere in den read/write wait_queues wartende Threads aufgeweckt.
 * Wartende Leser können nun aus dem Buffer lesen.
 * Ist der Buffer nicht voll, können wartende Schreibende Anfragen durchgeführt werden.

### Unterschied read/write
* Der write thread bekommt mit

  > sched_setscheduler(task, SCHED_RR, &param);
  
 eine höhere Priorität.
* Natürlich müssen bei der wake_up Bedingung jeweils die richtigen is_full und is_not_full Abfragen gemacht werden.

## Synchronisierung

Beim Eintritt in die write Funktion wird der globale Mutex gesetzt. Ist der Buffer beim nicht blockierenden Zugriff voll, wird dieser wieder released. Beim blockierenden Zugriff wird vor dem Aufruf von **wait_event_interruptible** dieser Mutex released, damit andere (Lese)-Zugriffe auf den Buffer möglich sind.

Werden mehrere Threads aus dieser wait_queue geweckt, muss sichergestellt werden, dass erst nur ein Thread schreiben darf. Sonst schreibt ein Thread den Buffer voll, und alle anderen Zugriffe würden dann fehlschlagen und aus der Funktion zurückkehren (obwohl der Zugriff blockierend ist).
Dieses Problem wird mit einer Schleife gelöst:
```C
while (buf_isfull(&dev_buf)) {
        mutex_unlock(&mutex);
        retval = wait_event_interruptible(write_wait_queue, !buf_isfull(&dev_buf));

        if(retval == -ERESTARTSYS)
                return -ERESTARTSYS;
        mutex_lock(&mutex);
}
```

Das wait_for_completion() wird benötigt, damit der gestartete Thread vor Aufruf von kthread_stop() gescheduled werden kann. Wird ansonsten kthread_stop() vor der Ausführung des Threads aufgerufen (was je nach Scheduling vorkommen kann), wird der Thread gar nicht erst ausgeführt.

## Kritik
* Da der Buffer intern auch einen Mutex verwendet, werden in einigen Fällen Mutexe unnötigerweise doppelt verwendet.
  Damit der Buffer aber universell threadsicher einsetzbar ist wird dies in Kauf genommen.
