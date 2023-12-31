# Разработка и оптимизация главного меню многопользовательской игры.

## Реализация

Основой разрабатываемого проекта послужил уже созданный прототип игры с возможностью подключения двух игроков по сети. Основная работа реализация меню и его оптимизация под различные устройства.

В качестве основы фона главного меню был взят готовый проект ArchViz Interior из каталога Epic Games Store. Этот проект представляет собой комнату с мебелью, настроенными материалами и текстурами в высоком разрешении и подготовленным освещением. Однако из-за большого количества объектов и материалов и их проработанности, рендеринг подобного проекта требует больших вычислительных затрат. 

При запуске приложения поверх подвижного изображения комнаты всегда отображаются кнопки главного меню, созданные с помощью UMG виджетов. Каждый переход по меню с помощью кнопок влияет на отображение фона. 

### Реализация фона в виде 3D-сцены

Реализация данного способа основывается на использовании сцены из проекта ArchViz Interior без изменений. При переходах между вкладками меню – меняется положение камеры. Для этого при запуске приложения на карте создаются 5 камер (по числу вкладок меню). Далее в C++ коде были добавлены строки с плавным изменением положения камеры функцией SetViewTargetWithBlend(), время “смешивания” которой было выставлено в 1.5 секунды.

При нажатии кнопок в меню срабатывает функция и камера плавно перемещается в новое место. При этом наблюдается практически вся сцена и наблюдатель ощущает глубину сцены благодаря полному рендерингу всей комнаты. 

Однако уже на этапе создания сцены и тестирования, было замечено, что частота кадров, даже на мощном компьютере не может приблизится даже к допустимым 60 кадрам в секунду.

### Реализация фона в виде статического изображения

Для данного варианта фона были сделаны несколько скриншотов комнаты из проекта ArchViz Interior в высоком разрешении. Реализация фона со статическим изображением возможна двумя способами. Первый, подставить скриншоты комнаты на фон как виджет-картинка в UMG. Второй, в 3D-сцене расположить плоскости с натянутыми на них текстурами скриншотов. Также во втором варианте можно реализовать эффект смещения камеры с помощью актера CameraShake.

Вариант реализации через картинки UMG – наиболее простой с точки зрения реализации, но и менее интересный с точки зрения визуального оформления. Достаточно “обернуть” виджеты кнопок Overlay и добавить в него на фон картинку нужной части комнаты. Далее при переключении Switcher картинка, как и кнопки будут правильно менятся.

Для второго варианта реализации на основе заранее сделанных скриншотов в окне editor Unreal Engine были созданы текстуры, а затем и материалы для отображения скриншотов на объектах. Далее материалы были применены к  нескольким объектам plane и помещены в пустую 3D-сцену вместе с соответветветствующим количеством камер. Для изменения изображения на фоне, была применена та же функция  SetViewTargetWithBlend(), только время изменения положения точки просмотра выставлено в 0 секунд, для мгновенного перехода.

Для эффекта движения камеры, при нахождении на одной вкладке меню применялся класс CameraShake. Задав амплитуду и частоту колебаний по оси X в некотором диапазоне случайных величин, был достигнут необходимый эффект с плавным движением камеры вверх-вниз.

Другим способом создать эффект движения камеры является создание новых условий в Event Graph виджета меню (см. рисунок 2). Изменяя координаты центра картинок по синусоидальному закону с повторением достигается требуемый эффект смещения камеры вверх-вниз по оси Y.

![.](/Content/Images/424626.png)

Рисунок 2 – Event Graph для эффекта движения камеры.

### Реализация фона в виде видео
Отображение видео на фоне меню также возможно двумя способами: картинка-видео в UMG или медиа-текстура на объекте plane в 3D-сцене. Оба варианта требуют последовательное создание трех сущностей в окне editor Unreal Engine: File Media Source, Media Player и Media Texture. Для варианта только с виджетами достаточно добавить медиа-текстуру в виде картинки в окне редактора UMG; если же текстура должна быть на плоскости в сцене, то дополнительно создается материал из данной текстуры. 

Для того, чтобы медиа-текстура начала работать, то есть отображать видео, необходимо добавить Blueprint в котором будет задействован узел Open Source – в данном случае ресурсом является File Media Source. На рисунке 3 представлена Event Graph для “активации” медиа-текстуры. Узел Main Video указывает на Media Player данного видео файла, а весь фрагмент срабатывает при создании сцены. В зависимости от того, где будет располагаться медиа-текстура, Event Graph нужно будет менять либо в окне виджетов UMG, либо в Level Blueprint сцены.

При запуске приложения сразу же начнет проигрываться видео. В не зависимости от переключения вкладок меню, видео не будет прерываться. 

Данный вариант реализации создает динамический фон, который показывает весь интерьер и не требует больших вычислительных ресурсов.

![.](/Content/Images/2023-07-18.png)

Рисунок 3 – Event Graph для отображения видео на медиа-текстуре.

### Оптимизация меню под различные экраны

Так как разрабатываемое приложение может быть запущено на различных устройствах, будь то персональный компьютер или ноутбук с широкоформатным монитором или небольшой смартфон; необходимо, чтобы меню приложения подстраивалось под любые соотношения сторон экрана и корректно отображает информацию. 

Для этого во всех виджетах через редактор UMG были добавлены Scale Box в режиме scale to fill к каждой картинке, отображающей фон, благодаря чему при вертикальной ориентации экрана фон будет не сжиматься, а обрезаться, что визуально корректнее. 

Для самих кнопок меню была также выбрана конфигурация ориентированная на “книжную” ориентацию экрана. Для этого вертикальные размеры Size Box, в котором находятся кнопки, превосходят горизонтальные. Для аккуратного масштабирования меню реализована взаимная вложенность двух типов виджетов Scale Box и Horizontal/Vertical Box. Horizontal/Vertical Box отвечают за рамки, за которые меню не сможет зайти никак. Scale Box позволяет меню масштабироваться при изменении ориентации экрана так, чтобы это выглядело “аккуратно”.

### Сборка проекта под Android и тестирование

Создание приложения для ОС Android с помощью Unreal Engine очень распространено, поэтому функционал для этого встроен в editor EU 4. Однако, так как Unreal Engine работает в основном с С++ кодом, требуются инструменты, чтобы перевести его для Android. Этими инструментами являются SDK и NDK. 

SDK (software development kit) — это набор инструментов для разработки программного обеспечения в одном устанавливаемом пакете. Они облегчают создание приложений, имея компилятор, отладчик и иногда программную среду. Некоторые SDK необходимы для разработки приложений на конкретную платформу, в данном случае на ОС Android.

Android NDK (Native Development Kit) – более специализированный, в сравнении с SDK, набор инструментов, которые помогают операционной системе Android работать с кодом, написанным на языках C и C++.

Для данного проекта были установлены с помощью приложения Android Studio были установлены следующие версии SDK и NDK:
- SDK Platform – Android API 32
- SDK Build-tool  29.0.2
- NDK 21.4.7075529

### Результаты

После успешной упаковки файлов проекта, было проведено тестирование его производительности.	Для тестирования был использован смартфон компании Samsung Galaxy A51 2019 года с операционной системой Android 10 версии. Результаты тестирования приведены в виде столбчатых диаграмм на рисунках 4 и 5 и в виде таблице 1 с абсолютными значениями. 

![.](/Content/Images/charge_1.png)

Рисунок 4 – Относительное процентное соотношение расхода заряда смартфона за 10 мин работы

![.](/Content/Images/fps_1.png)

Рисунок 5 – Относительное процентное соотношение fps при работе 
