# BufferQueue机制

## 1. 类的关系

### 1.1 BufferQueue

#### 1.1.1. 引入

`BufferQueue` 的原始生产者和消费者分别为 `BufferQueueProducer` 和 `BufferQueueConsumer`，其关系如下图所示。

![1](picture/1.png)

##### BufferQueueProducer
生产者通过 `RequestBuffer()` 从 `BufferQueue` 中请求一个可用的 buffer，
绘制完毕后调用 `FlushBuffer()` 送显，并回调消费者监听回调函数 `OnBufferAvailable()`。

##### BufferQueueConsumer 
消费者收到通知后调用 `AcquireBuffer()` 获取 buffer，
使用完后调用 `ReleaseBuffer()` 释放，并回调生产者监听回调函数 `OnBufferRelease()`。

##### BufferQueue
+ 申请内存：当 `BufferQueue` 中的队列中没有可重复使用的 buffer 时，会调用 `AllocBuffer()` 申请内存，会通过创建 `SurfaceBufferImpl` 申请内存，并赋给新的 `BufferElement`。
`SurfaceBufferImpl` 是抽象类 `SurfaceBuffer` 的实现类。

+ 释放内存：调用 `DeleteBuffers`，将 `BufferQueue` 中的 `freeList_` 和 `dirtyList_` 中的元素清空，将 `bufferQueueCache_` 中所映射到的 `BufferElement` 对象的 `isDeleting` 置 `True`。

##### Buffer 状态
同时，buffer 的状态随生产者、消费者的操作而改变，其状态转移关系如下图所示。

![2](picture/2.png)

#### 1.1.2. BufferQueue 相关的类

`BufferQueueProducer` 和 `BufferQueueConsumer` 均包含一个指向 `BufferQueue` 对象的指针，通过 `BufferQueue` 对象操作内存。

![3](picture/3.png)

#### 1.1.3. Surface 相关的类

抽象类 `Surface` 是对 `BufferQueue` 直接生产者和消费者的封装。
`ProducerSurface` 和 `ConsumerSurface` 是 `Surface` 的实现类，分别代表封装后的生产者和消费者。

`ConsumerSurface` 包含一个指向 `BufferQueueProducer` 和 `BufferQueueConsumer` 的指针。
`ProducerSurface` 包含一个指向 `IBufferProducer` 的指针。

![4](picture/4.png)

### 1.2. Producer 相关的类

`RSSurfaceOhosGl` 可视为处于用户端的生产者。

`RSSurfaceOhosGl` 类包含一个指向 `ProducerSurface` 的指针 `producer_`，该指针包含一个指向 `BufferClientProducer` 的指针 `producer_`，
`BufferClientProducer` 对象可通过IPC远程调用 `BufferQueueProducer` 的函数，请求或者发送 buffer 到 `BufferQueue`。

![5](picture/5.png)

### 1.3. Consumer 相关的类

`HdiFramebufferSurface` 可视为消费者。

`HdiFramebufferSurface` 类包含一个指向 `ConsumerSurface` 的指针 `consumerSurface_`，该指针包含一个指向 `BufferQueueConsumer` 的指针 `consumer_`，对应着 `BufferQueue` 的直接消费者。
 
![6](picture/6.png)
