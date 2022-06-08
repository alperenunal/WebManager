# WebManager

## Genel

WebManager internet tarayıcısında çalışan htop benzeri bir uygulamadır.

Uygulama sadece Linux sistemlerinde çalışmak üzere tasarlanmıştır.

## Kurulum

WebManager işlemci sıcaklığı verilerine ulaşabilmek için libsensors kütüphanesini ve sensors-detect uygulamasını kullanır.

Kurulum için bulunan make dosyası apt paket yöneticisini kullanarak gerekli bağımlılıkları indirir. Apt paket yöneticisini kullanmayan Linux dağıtımları bu paketleri kendileri indirmelidir.

Ubuntu, Debian vb. sistemler için basitçe

    make

komutunu çalıştırmak gerekli paketleri indirip, programı derleyip başlatır.

## Kullanım

WebManager çalıştırılmadan önce sensors-detect uygulaması başlatılıp ilk sunulan sensör verilerinin taranmasına izin verilmelidir.

WebManager varsayılan port olarak 8000'i kullanır. Program başlatılmadan önce bu isteğe göre değiştirilebilir.

    webmanager -p <port>

- WebManager program seçenekleri için tek tire kullanır. Örneğin yardım için

        webmanager -h

Arayüze ulaşmak için tarayıcıda

    localhost:port

adresine gidilmelidir.

## Arayüz

Uygulama arayüzü 5 ana bölümden oluşur.

1. bölümde sistemle ilgili genel bilgiler görüntülenebilir, ve kapatma,
   yeniden başlatma seçenekleri bulunur.

2. bölümde shell komutları çalıştırılabilir veya WebManager tarafından
   başlatılmış ve hala aktif durumda olan programlar isim veya process id'leri girilerek sonlandırılabilir.

3. bölümde WebManager tarafından çalıştırılmış programlar bazı bilgileriyle
   birlikte gösterilir.

4. bölümde program ayarları bulunur. Seçenekler:

   - bilgilerin güncellenme süresi
   - uyarı mesajları alınması için gereken işlemci kullanımı
   - uyarı mesajları alınması için gereken işlemci sıcaklığı
   - kayıtlarda en fazla görüntülenecek mesaj sayısı

5. bölümde program süresince gerçekleşen olayların kayıtları bulunur.

Ayrıca Exit butonuna basılarak program bitirilebilir.

WebManager içersinde web server ve API bulundurur.

## API

    GET / -> index.html

    GET /style.css -> style.css

    GET /index.js -> index.js

    GET /info -> JSON

        {
            "uptime": ,
            "temp": ,
            "load-avg": ,
            "loads": ,
            "sec": ,
            "entry": ,
            "procs": ,
            "logs":
        }

    POST / -> index.html

        sys-cmd=s
        sys-cmd=r
        sys-cmd=c

        run-cmd=string
        kill-cmd=string

        sec-set=number
        util-set=number
        temp-set=number
        entry-set=number

        clear-logs=
