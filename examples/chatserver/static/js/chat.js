$(function() {

    var index = -1,
        $document = $(document),
        $messages = $('.messages');

    // Retrieve all messages after the specified index
    function update() {
        $.ajax({
            type: 'POST',
            url: '/api/getMessages',
            data: JSON.stringify({index: index}),
            contentType: 'application/json',
            complete: function() {
                window.setTimeout(update, 2000);
            },
            success: function(data) {
                $.each(data.messages, function(i, e) {
                    $('<div>')
                        .addClass('message')
                        .text(e.message)
                        .appendTo($messages);
                    index = e.index;
                });
                $document.scrollTop(10000);
            }
        });
    }

    update();

    // Find the message input box and set the appropriate handler for [enter]
    var $input = $('#input').focus().keypress(function(e) {
        if(e.which == 13) {
            $.ajax({
                type: 'POST',
                url: '/api/postMessage',
                data: JSON.stringify({message: $(this).val()}),
                contentType: 'application/json'
            });
            // Clear the contents
            $(this).val('');
            return false;
        }
    });
});
