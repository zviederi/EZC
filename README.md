EZC
Dažas lietones, kurās tiek izmantots OpenCL.
====
OpenCL lietotnes

OPENCL IZMANTOŠANA LIETOJUMOS

Skaitļošanas uzdevumus ļoti labi izpilda CPU, bet ja uzdevumā ir liels elementu apstrādes skaits, tad skaitļošana var ievilkties. Izmantojot heterogēnās sistēmas ir iespējams panākt to, ka uzdevumi ar apjomīgiem apstrādes datiem tiek izpildīti paralēli un uz optimālās platformās, kas atrodas atbilstošajā darbstacijā. Jauns un industriāls izstrādes ietvars – OpenCL, adresēts tiem skaitļošanas uzdevumiem, kuros ir liels apstrādes elementu skaits. OpenCL nodrošina augstu pārnesamību, izspiežot no aparatūras maksimālo. Pētījuma mērķis ir noskaidrot OpenCL darbības principu un kādos algoritmos labāk ir pielietot. Rezultātā ir vairāki analīžu rezultāti par dažādiem skaitļošanas algoritmiem, kur jau tagad ir zināms, ka OpenCL efektivitāte parādās pie lieliem datu apjomiem, kā arī vispārēji secinājumi par OpenCL ietvaru.
====
EZC
Some applications, where use OpenCL.
====
OpenCL applications

USE OF OPENCL IN APPLICATIONS

CPU is good at executing sequential calculation tasks, but what happens when the number of elements to be calculated is very high? Traditional CPU-based computation may take a long time. This problem can be fixed with the help of heterogeneous system. The heterogeneous systems allows a programmer to write applications that seamlessly integrate CPUs with GPUs, while benefiting from the best attributes of each. The heterogeneous systems have become an important class of platforms, and OpenCL framework is the first industry standard that directly addresses their needs. OpenCL delivers high levels of performance by exposing the hardware, not by hiding it behind elegant abstractions. This means that the OpenCL programmer must explicitly define the platform, its context, and how work is scheduled onto different devices. The goal of this research is to show how OpenCL works and which algorithms are most suited for OpenCL calculation. As the result, we describe several analysis and conclusions about OpenCL efficiency.

